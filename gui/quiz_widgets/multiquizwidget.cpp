#include "multiquizwidget.h"
#include "quizareadisplay.h"
#include "quizrepresentationwidget.h"
#include "quiziconwidget.h"
#include "singlequizwidget.h"
#include <QPushButton>

MultiQuizWidget::MultiQuizWidget(QWidget *parent) : QStackedWidget(parent)
  ,display(new QuizAreaDisplay(this))
  ,representationWidget(new QuizRepresentationWidget(this))
  ,quizWidget(new SingleQuizWidget(this))
{
    this->addWidget(display);
    this->addWidget(representationWidget);
    this->addWidget(quizWidget);


    connect(display->returnButton(), &QPushButton::clicked, this, &MultiQuizWidget::backToMenu);


    connect(display, &QuizAreaDisplay::startQuizClicked, this, &MultiQuizWidget::onQuizStart);


    connect(representationWidget->returnButton(), &QPushButton::clicked, this, [=](){
        this->setCurrentWidget(display);
    });
    connect(representationWidget->startButton(), &QPushButton::clicked, this, [=](){
        this->setCurrentWidget(quizWidget);
        quizWidget->setQuiz(representationWidget->getQuiz());
        quizWidget->startTimer();
    });

    connect(quizWidget, &SingleQuizWidget::quizFinished, this, [=]{
        this->setCurrentWidget(this->display);
    });

}

void MultiQuizWidget::setQuiz(const QList<singleQuizPtr> &list)
{
    this->quizes = list;
    if(list.isEmpty()) return;
    setupGui();
}

void MultiQuizWidget::addQuiz(const singleQuizPtr &quiz)
{
    this->quizes.append(quiz);
    this->display->addQuiz(quiz);
}

void MultiQuizWidget::removeQuiz(const singleQuizPtr &quiz)
{
    this->quizes.removeAll(quiz);
    this->display->removeQuiz(quiz);
}

void MultiQuizWidget::onQuizStart(singleQuizPtr quiz)
{
    this->representationWidget->setQuiz(quiz);
    this->setCurrentWidget(representationWidget);
}

void MultiQuizWidget::setupGui()
{
    this->display->clearAndRemoveAllIcons();
    for(const singleQuizPtr& ptr: qAsConst(quizes)){
        this->display->addQuiz(ptr);
    }
    this->setCurrentWidget(display);
}
