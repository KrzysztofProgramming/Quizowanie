#include "quizcreatorwidget.h"
#include "./gui/quiz_widgets/createquizwidget.h"
#include "./gui/question_widgets/createquestionwidget.h"
#include "./gui/question_widgets/questionlistelement.h"
#include "./gui/mainwindow.h"
#include "./logic/database/databasemanager.h"
#include <QMessageBox>
#include <QFutureWatcher>
#include <QLabel>
#include <QProgressBar>
#include "./gui/simple_widgets/busylabel.h"

QuizCreatorWidget::QuizCreatorWidget(MainWindow *mainWindow, QWidget *parent)
    : QStackedWidget((parent == nullptr) ? mainWindow : parent)
    , quizCreator(new CreateQuizWidget(this))
    , questionCreator(new CreateQuestionWidget(this))
    , busyLabel(new BusyLabel(this))
    , watcher(new QFutureWatcher<bool>(this))
    , window(mainWindow)
{
    this->addWidget(quizCreator);
    this->addWidget(questionCreator);
    this->addWidget(busyLabel);

    const QList<singleQuizPtr>& customQuizes = window->getCustomQuizes();
    QStringList forbiddenNames;
    for(int i=0 ;i<customQuizes.size(); i++){
        forbiddenNames.append(customQuizes.at(i)->getTitle());
    }
    quizCreator->setForbiddenTitles(forbiddenNames);

    connect(quizCreator, &CreateQuizWidget::addNewQuestionClicked, this, &QuizCreatorWidget::onAddingQuestion);
    connect(questionCreator, &CreateQuestionWidget::questionConfirmed, this, &QuizCreatorWidget::onConfirmNewQuestion);
    connect(questionCreator, &CreateQuestionWidget::questionDenied, this, &QuizCreatorWidget::onNewQuestionDenied);
    connect(quizCreator, &CreateQuizWidget::startEditQuestion, this, &QuizCreatorWidget::onEditingQuestion);
    connect(quizCreator, &CreateQuizWidget::quizDeny, this,[=](){
        emit this->back(!this->quizToEdit.isNull());
    });
    connect(quizCreator, &CreateQuizWidget::quizConfirm, this, &QuizCreatorWidget::onQuizConfirmed);
    connect(watcher, &QFutureWatcher<bool>::finished, this, &QuizCreatorWidget::onWatcherFinished);
}

singleQuizPtr QuizCreatorWidget::getQuiz() const
{
    return quizCreator->getQuiz();
}

void QuizCreatorWidget::setQuiz(const singleQuizPtr &quiz)
{
    this->setCurrentWidget(quizCreator);
    this->quizCreator->setQuiz(quiz);
    this->quizToEdit = nullptr;
}

void QuizCreatorWidget::reset()
{
    this->setCurrentWidget(quizCreator);
    this->quizCreator->reset();
    this->quizToEdit = nullptr;
}

void QuizCreatorWidget::onAddingQuestion()
{
    this->setCurrentWidget(questionCreator);
    questionCreator->reset();
    editingElement = nullptr;
}

void QuizCreatorWidget::onNewQuestionDenied()
{
    this->setCurrentWidget(quizCreator);
    editingElement = nullptr;
}

void QuizCreatorWidget::onConfirmNewQuestion(basicQuestionPtr question)
{
    this->setCurrentWidget(quizCreator);
    if(editingElement!=nullptr){
        editingElement->setQuestion(question);
        editingElement = nullptr;
    }
    else{
        quizCreator->addQuestion(question);
    }
}

void QuizCreatorWidget::onEditingQuestion(QuestionListElement *element)
{
    editingElement = element;
    this->setCurrentWidget(questionCreator);
    questionCreator->setQuestion(element->getQuestion());
}

void QuizCreatorWidget::onQuizConfirmed(singleQuizPtr quiz)
{
    if(future.isRunning()) return;
    DatabaseManager *manager = window->getCustomDbManager();
    good = true;
    this->quizFromConfirmed = quiz;

    if(!this->quizToEdit.isNull()){
        if((*quizToEdit.get())==(*quiz.get()) && !quizCreator->hasImageChanged() && !questionCreator->hasImageChanged()){
            emit back(true);
            return;
        }
        good = manager->removeQuiz(quizToEdit->getTitle());
    }
    future = manager->saveQuizAsync(quiz);
    watcher->setFuture(future);
    this->setCurrentWidget(busyLabel);
}

void QuizCreatorWidget::onWatcherFinished()
{
    if(good && future){

        if(this->quizToEdit.isNull())QMessageBox::information(this, tr("Sukces"), tr("Pomyslnie zapisano twój quiz!"));
        else QMessageBox::information(this, tr("Sukces"), tr("Pomyslnie zmieniono twój quiz!"));

        if(!this->quizToEdit.isNull()){
            emit this->quizEdited(this->quizToEdit, this->quizFromConfirmed);
        }
        else emit this->quizSaved(this->quizFromConfirmed);
        emit back(!this->quizToEdit.isNull());
    }
    else if(!this->quizToEdit.isNull()){
        QMessageBox::critical(this, tr("Niepowodzenie"), tr("Nie udało się edytować quizu, spróbuj zmienić jego nazwę lub sprawdź, "
                                                            "czy nie ma otwartych plików w jego folderze"));
        return;
    }else{
        QMessageBox::critical(this, tr("Niepowodzenie"), tr("Nie udało się zapisać quizu, spróbuj zmienić jego nazwę"));
        return;
    }
    this->quizToEdit = nullptr;
    this->quizFromConfirmed = nullptr;
}
