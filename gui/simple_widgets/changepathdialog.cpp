#include "changepathdialog.h"
#include "ui_changepathdialog.h"
#include <QFileDialog>
#include <QMessageBox>

ChangePathDialog::ChangePathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePathDialog)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    this->setWindowTitle(tr("Wybierz ścieżkę"));

    this->okButton = this->ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
    this->cancelButton = this->ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel);

    connect(this->ui->pathButton, &QPushButton::clicked, this, &ChangePathDialog::onPathButtonClicked);
    connect(okButton, &QPushButton::clicked, this, &ChangePathDialog::onAcceptButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ChangePathDialog::reject);
}

ChangePathDialog::~ChangePathDialog()
{
    delete ui;
}

void ChangePathDialog::setError(const QString &error)
{
    this->ui->errorLabel->setText(error);
}

QString ChangePathDialog::lastError()
{
    return this->ui->errorLabel->text();
}

void ChangePathDialog::setDisplayPath(const QString &path)
{
    this->ui->pathEdit->setText(path);
    if(path.isEmpty()) this->setError("Nie podano ścieżki");
    else if(commentFunction!=nullptr){
        this->setError(commentFunction(path));
    }
    else checkPath(path);
}

void ChangePathDialog::setPath(const QString &path)
{
    this->setDisplayPath(QDir::toNativeSeparators(path));
    startPath = path;
}

QString ChangePathDialog::getDisplayPath() const
{
    return QDir::fromNativeSeparators(this->ui->pathEdit->text());
}

void ChangePathDialog::setDescription(const QString &text)
{
    this->ui->infoLabel->setText(text);
}

QString ChangePathDialog::getDescription() const
{
    return this->ui->infoLabel->text();
}

void ChangePathDialog::onPathButtonClicked()
{
    QString dirPath;
    if(selectingFiles){
        dirPath = QFileDialog::getOpenFileName(this, tr("Wybierz plik"));
    }
    else dirPath = QFileDialog::getExistingDirectory(this, tr("Wybierz folder"));
    if(dirPath.isEmpty()) return;
    setDisplayPath(dirPath);
}

void ChangePathDialog::onAcceptButtonClicked()
{
    bool accept = true;


    if(this->getDisplayPath().isEmpty()){
        QMessageBox::information(this, tr("Pusta ścieżka"), tr("Podaj ścieżkę, aby kontynuować"));
        accept = false;
    }
    else if(this->getDisplayPath() == this->getPath()){
        accept = true;
    }
    else if(checkingFunction!=nullptr && !checkingFunction(this->getDisplayPath())){
        accept = false;
    }

    if(accept){
        this->startPath = getDisplayPath();
        this->accept();
    }
}
void ChangePathDialog::checkPath(const QString &path)
{
    if(path.isEmpty()) return;
    QDir dir(path);
    if(!dir.exists()){
        this->setError(tr("Taki folder nie istnieje!"));
        return;
    }
    if(!noEmptyError && !dir.isEmpty() && path!=this->startPath){
        this->setError(tr("Ten folder nie jest pusty"));
        return;
    }
}

int ChangePathDialog::exec()
{
    this->setError("");
    return QDialog::exec();
}
