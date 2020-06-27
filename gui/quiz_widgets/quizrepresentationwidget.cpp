#include "quizrepresentationwidget.h"
#include "ui_quizrepresentationwidget.h"
#include <QResizeEvent>
#include "./gui/simple_widgets/pixmaplabel.h"

QuizRepresentationWidget::QuizRepresentationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuizRepresentationWidget)
  , imgLabel(new PixmapLabel(this))
{
    ui->setupUi(this);
    this->ui->verticalLayout->insertWidget(1, imgLabel);
    imgLabel->setText("");
    imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QuizRepresentationWidget::~QuizRepresentationWidget()
{
    delete ui;
}

void QuizRepresentationWidget::setQuiz(singleQuizPtr quiz)
{
    this->quiz = quiz;
    refreshUI();
}

void QuizRepresentationWidget::refreshUI()
{
    //title
    ui->titleLabel->setText(quiz->getTitle());

    //image
    if(quiz->hasImage()){
        imgLabel->show();
        imgLabel->setPixmap(quiz->getTitleImage());
    }
    else{
        imgLabel->hide();
    }

    //questionsCount
    ui->questionCountLabel->setText(tr("Liczba wszystkich pytań: ") + QString::number(quiz->getQuestions().size()));

    //time
    if(quiz->isTimeSpecified()){
        ui->timeLabel->setText(tr("Czas na wykonianie tego quizu to: ") + quiz->getMaxTime().toString("hh:mm:ss"));
    }

    else{
        int timeQuestionCount = 0;
        for(const auto& x: quiz->getQuestions()){
            if(x->isTimeSpecified()) ++timeQuestionCount;
        }
        if(timeQuestionCount == 0) ui->timeLabel->setText(tr("Nie ma limitu czasowego"));
        else if(timeQuestionCount == quiz->getQuestions().size()) ui->timeLabel->setText(tr("Każde pytanie ma osobny limit czasowy"));
        else ui->timeLabel->setText("Niektóre pytania mają ustalony limit czasowy");
    }

    //points
    ui->pointsCountLabel->setText(tr("Możliwe punkty do uzyskania: ") +QString::number(quiz->getMaxPoints()));
}

QPushButton *QuizRepresentationWidget::returnButton() const
{
    return this->ui->returnButton;
}

QPushButton *QuizRepresentationWidget::startButton() const
{
    return this->ui->startButton;
}
