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
#include "./logic/database/quizstore.h"

QuizCreatorWidget::QuizCreatorWidget(MainWindow *mainWindow, QWidget *parent)
    : QStackedWidget((parent == nullptr) ? mainWindow : parent)
    , quizCreator(new CreateQuizWidget(this))
    , questionCreator(new CreateQuestionWidget(this))
    , window(mainWindow)
{
    this->addWidget(quizCreator);
    this->addWidget(questionCreator);

    //setting forbidden names
    quizCreator->setForbiddenTitles(mainWindow->getCustomQuizStore()->getActualFoldersNames());

    connect(quizCreator, &CreateQuizWidget::addNewQuestionClicked, this, &QuizCreatorWidget::onAddingQuestion);
    connect(questionCreator, &CreateQuestionWidget::questionConfirmed, this, &QuizCreatorWidget::onConfirmNewQuestion);
    connect(questionCreator, &CreateQuestionWidget::questionDenied, this, &QuizCreatorWidget::onNewQuestionDenied);
    connect(quizCreator, &CreateQuizWidget::startEditQuestion, this, &QuizCreatorWidget::onEditingQuestion);
    connect(quizCreator, &CreateQuizWidget::quizDeny, this,[=](){
        emit this->back(!this->quizToEdit.isNull());
    });
    connect(quizCreator, &CreateQuizWidget::quizConfirm, this, &QuizCreatorWidget::onQuizConfirmed);
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
    if(!this->quizToEdit.isNull()){
        if((*quizToEdit.get())==(*quiz.get()) && !quizCreator->hasImageChanged() && !questionCreator->hasImageChanged()){
            emit back(true);
            return;
        }
        emit this->quizEdited(this->quizToEdit, quiz);
        return;
    }
    emit this->quizSaved(quiz);
}
