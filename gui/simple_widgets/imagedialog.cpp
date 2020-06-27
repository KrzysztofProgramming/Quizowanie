#include "imagedialog.h"
#include "ui_imagedialog.h"
#include <QFileDialog>
#include <QImageReader>
#include "pixmaplabel.h"


ImageDialog::ImageDialog(int width, int height, QWidget *parent)
    :ImageDialog(parent)
{
    this->resize(width, height);
}

QStringList ImageDialog::filters;

ImageDialog::ImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
  , dialog(new QFileDialog(this))
  , imgLabel(new PixmapLabel(this))

{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    this->ui->verticalLayout->insertWidget(2, imgLabel);
    imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWindowTitle(tr("Wybierz obraz"));


    static bool firstTime = true;
    if(firstTime){
        firstTime = false;
        QList<QByteArray> filterList = QImageReader::supportedMimeTypes();
        for(const QByteArray& filter: qAsConst(filterList)){
            filters.append(filter);
        }
        filters.sort();
    }
    dialog->setMimeTypeFilters(filters);
    dialog->selectMimeTypeFilter("image/png");

    connect(this->ui->selectImgButton, &QPushButton::clicked, this, &ImageDialog::selectImage);
    connect(this->ui->buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ImageDialog::accept);
    connect(this->ui->buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &ImageDialog::reject);
    connect(this->ui->removeImgButton, &QPushButton::clicked, this, [=](){
       this->setPixmap(QPixmap());
    });
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::setPixmap(const QPixmap &pixmap)
{
    this->imgLabel->setPixmap(pixmap);
}

const QPixmap &ImageDialog::getPixmap()
{
    return this->imgLabel->getPixmap();
}

void ImageDialog::setDescription(const QString &description)
{
    this->ui->descriptionLabel->setText(description);
}

void ImageDialog::selectImage()
{
    dialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    dialog->exec();
    QStringList list = dialog->selectedFiles();
    QString imgPath = (list.isEmpty()) ? "" : list.first();
    if(imgPath.isEmpty()) return;
    QFileInfo inf(imgPath);
    if(inf.isFile()){
        QPixmap temp;
        temp.load(imgPath);
        if(temp.isNull()){
            Q_ASSERT(true);
            return;
        }
       this->imgLabel->setPixmap(temp);
    }
}
