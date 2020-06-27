#include "quizsummarywidget.h"
#include "ui_quizsummarywidget.h"
#include <QTime>
#include "./gui/simple_widgets/pixmaplabel.h"

QString QuizSummaryWidget::percentToGrade(double percent)
{
    if(percent>=min6) return "6";
    else if(percent >= min5) return "5";
    else if(percent >= min4plus) return "+4";
    else if(percent >= min4) return "4";
    else if(percent >= min3plus) return "+3";
    else if(percent >= min3) return "3";
    else if(percent >= min2) return "2";
    else return "1";
}

QuizSummaryWidget::QuizSummaryWidget(singleQuizPtr quiz, int pointsGot, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuizSummaryWidget)
  , imgLabel(new PixmapLabel(this))
  ,points(pointsGot)
{
    ui->setupUi(this);
    setQuiz(quiz);
    setPoints(pointsGot);
    imgLabel->setText("");
    imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->ui->mainLayout->insertWidget(ui->mainLayout->count() - 1, imgLabel);

    connect(ui->confirmButton, &QPushButton::clicked, this, &QuizSummaryWidget::confirmed);
}

QuizSummaryWidget::~QuizSummaryWidget()
{
    delete ui;
}

void QuizSummaryWidget::setQuiz(singleQuizPtr quiz)
{
    this->quiz = quiz;
    if(quiz.isNull()) return;
    setupGui();
}

void QuizSummaryWidget::setPoints(int pointsGot)
{
    this->points = pointsGot;
    this->ui->pointsLabel->setText(tr("Twój wynik to: ") + QString::number(points) + '/' +
                                   ((quiz.isNull()) ? QString() : QString::number(quiz->getMaxPoints())));
    if(!quiz.isNull())
        this->ui->gradeLabel->setText(QuizSummaryWidget::percentToGrade((this->points*100.0)/quiz->getMaxPoints()));
}

void QuizSummaryWidget::setTime(const QTime &time)
{
    if(!time.isNull()){
        ui->timeLabel->show();
        ui->timeLabel->setText(tr("Twój czas: ") + time.toString("hh:mm:ss"));
    }
    else{
        ui->timeLabel->hide();
    }
}

void QuizSummaryWidget::setupGui()
{
    //image
    if(quiz->hasImage()){
        imgLabel->show();
        imgLabel->setPixmap(quiz->getTitleImage());
    }
    else
        imgLabel->hide();
    //grade
    this->ui->gradeLabel->setText(QuizSummaryWidget::percentToGrade((this->points*100.0)/quiz->getMaxPoints()));
    //title
    this->ui->quizNameLabel->setText(tr("Gratulację przeszedłeś ten quiz!"));
}
