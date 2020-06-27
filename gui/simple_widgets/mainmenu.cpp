#include "mainmenu.h"
#include "ui_mainmenu.h"

MainMenu::MainMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenu)
{
    ui->setupUi(this);
    connect(this->ui->exitButton, &QAbstractButton::clicked, QApplication::instance(), &QApplication::closeAllWindows);
}

MainMenu::~MainMenu()
{
    delete ui;
}

QPushButton *MainMenu::startButton()
{
   return this->ui->startQuizButton;
}

QPushButton *MainMenu::startCustomButton()
{
    return this->ui->customQuizButton;
}

QPushButton *MainMenu::creatQuizButton()
{
    return this->ui->createQuizButton;
}

QPushButton *MainMenu::manageButton()
{
    return this->ui->manageButton;
}

void MainMenu::hideExitButton()
{
    this->ui->exitButton->hide();
}

void MainMenu::showExitButton()
{
    this->ui->exitButton->show();
}
