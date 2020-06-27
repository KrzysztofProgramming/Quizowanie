#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    this->setWindowTitle(tr("O Programie"));

    connect(this->ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

QString AboutDialog::getText() const
{
    return this->ui->infoLabel->text();
}

void AboutDialog::setText(const QString &text)
{
    this->ui->infoLabel->setText(text);
}
