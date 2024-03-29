#include "titlewidget.h"
#include "ui_titlewidget.h"
#include "mainmenu.h"
#include <QFont>
#include <QPushButton>
#include "./gui/mainwindow.h"
#include "gui/quiz_widgets/multiquizwidget.h"
#include "./gui/simple_widgets/quizcreatorwidget.h"
#include "./gui/quiz_widgets/quizareadisplay.h"
#include "./logic/database/databasemanager.h"
#include "./logic/database/quizstore.h"
#include "busylabel.h"
#include <QMessageBox>

TitleWidget::TitleWidget(MainWindow *parent) :
    QWidget(parent),
    mainWindow(parent)
  , ui(new Ui::TitleWidget)
  , mainMenu(new MainMenu(this))
  , defaultQuizes(new MultiQuizWidget(this, mainWindow->getDefaultQuizStore()))
  , customQuizes(new MultiQuizWidget(this, mainWindow->getCustomQuizStore()))
  , creator(new QuizCreatorWidget(parent, this))
  , busyLabel(new BusyLabel(this))
{
    ui->setupUi(this);
    QFont font = ui->titleLabel->font();
    font.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 10);
    ui->titleLabel->setFont(font);

    ui->stackedWidget->addWidget(mainMenu);
    ui->stackedWidget->addWidget(defaultQuizes);
    ui->stackedWidget->addWidget(customQuizes);
    ui->stackedWidget->addWidget(creator);
    ui->stackedWidget->addWidget(busyLabel);

    // customQuizes->setQuizes(mainWindow->getCustomQuizes());
    //defaultQuizes->setQuizes(mainWindow->getDefaultQuizStore()->getAllQuizes());ds

    connect(customQuizes, SIGNAL(backToMenu()), this, SLOT(backToMenu()));
    connect(defaultQuizes, SIGNAL(backToMenu()), this, SLOT(backToMenu()));
    connect(mainMenu->startButton(), &QPushButton::clicked, this, &TitleWidget::startQuiz);
    connect(mainMenu->startCustomButton(), &QPushButton::clicked, this, &TitleWidget::startCustomQuiz);
    connect(mainMenu->creatQuizButton(), &QPushButton::clicked, this, &TitleWidget::createCustomQuiz);
    connect(mainMenu->manageButton(), &QPushButton::clicked, this, &TitleWidget::managaQuizes);

    connect(creator, SIGNAL(back(bool)), this, SLOT(backToMenu(bool)));
    connect(creator, &QuizCreatorWidget::quizSaved, this, &TitleWidget::onQuizSave);
    connect(creator, &QuizCreatorWidget::quizEdited, this, &TitleWidget::onQuizEdited);
    connect(this->customQuizes->getDisplay(), &QuizAreaDisplay::removeQuizClicked,this, &TitleWidget::onQuizRemove);
    connect(this->customQuizes->getDisplay(), &QuizAreaDisplay::editQuizClicked,this, &TitleWidget::onQuizStartEdit);
    connect(this->mainWindow->getCustomQuizStore(), &QuizStore::quizAdded, this, &TitleWidget::onQuizSavingCompleted);
    connect(this->mainWindow->getCustomQuizStore(), &QuizStore::addingFailed, this, &TitleWidget::onQuizSavingFailed);
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::startQuiz()
{
    ui->stackedWidget->setCurrentWidget(defaultQuizes);
}

void TitleWidget::startCustomQuiz()
{
    customQuizes->getDisplay()->setIconsMode(QuizIconWidget::Display);
    ui->stackedWidget->setCurrentWidget(customQuizes);
}

void TitleWidget::createCustomQuiz()
{
    this->creator->reset();
    this->ui->stackedWidget->setCurrentWidget(creator);
}

void TitleWidget::managaQuizes()
{
    customQuizes->getDisplay()->setIconsMode(QuizIconWidget::Edit);
    this->ui->stackedWidget->setCurrentWidget(customQuizes);
}

void TitleWidget::backToMenu(bool fromEditMode)
{
    if(fromEditMode) managaQuizes();
    else this->ui->stackedWidget->setCurrentWidget(mainMenu);
}
void TitleWidget::onQuizSave(singleQuizPtr quiz)
{
    this->fromEditMode = false;
    this->currentlySavingQuiz = quiz;
    this->mainWindow->getCustomQuizStore()->addQuizAsync(quiz);
    this->ui->stackedWidget->setCurrentWidget(busyLabel);
}

void TitleWidget::onQuizStartEdit(singleQuizPtr quiz)
{
    this->creator->setQuizToEdit(quiz);
    this->ui->stackedWidget->setCurrentWidget(creator);
}

void TitleWidget::onQuizRemove(singleQuizPtr quiz)
{
    //here quiz must be deleted and check if user really wants it
    int value = QMessageBox::warning(this, tr("Usuwanie quizu"), tr("Czy na pewno chcesz usunąć ten quiz?"),
                                     QMessageBox::Yes | QMessageBox::Cancel);
    if(value!=QMessageBox::Yes) return;

    if(!mainWindow->getCustomQuizStore()->removeQuiz(quiz)){
        QMessageBox::critical(this, tr("Error"), tr("Nie udało się usunąć quizu, sprawdź czy w jego folderze nie ma żadnych otwartych plików"));
        return;
    }
}

void TitleWidget::onQuizEdited(singleQuizPtr oldVersion, singleQuizPtr newVersion)
{
    this->fromEditMode = true;
    bool value = this->mainWindow->getCustomQuizStore()->removeQuiz(oldVersion);
    if(value){
        this->currentlySavingQuiz = newVersion;
        this->mainWindow->getCustomQuizStore()->addQuizAsync(newVersion);
    }
    else onQuizSavingFailed(newVersion);
    this->ui->stackedWidget->setCurrentWidget(busyLabel);
}

void TitleWidget::onQuizSavingCompleted(singleQuizPtr quiz)
{
    if(quiz!=currentlySavingQuiz) return;
    currentlySavingQuiz = nullptr;

    if(fromEditMode){
        QMessageBox::information(this, tr("Sukces"), tr("Pomyślnie zmieniono twój quiz"));
        managaQuizes();
    }
    else{
        QMessageBox::information(this, tr("Sukces"), tr("Pomyślnie zapisano twój quiz"));
        this->ui->stackedWidget->setCurrentWidget(mainMenu);
    }
}

void TitleWidget::onQuizSavingFailed(singleQuizPtr quiz)
{
    if(quiz!=currentlySavingQuiz) return;
    currentlySavingQuiz = nullptr;
    QMessageBox::critical(this, tr("ERROR"), tr("Nie udało się zapisać quizu"));
    qDebug() <<"Nie udało się zapisać";
    this->ui->stackedWidget->setCurrentWidget(creator);
}
