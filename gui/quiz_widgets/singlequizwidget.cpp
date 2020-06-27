#include "singlequizwidget.h"
#include "ui_singlequizwidget.h"
#include "./logic/singlequestion.h"
#include "./logic/multiquestion.h"
#include "./gui/question_widgets/basicquestionwidget.h"
#include <QTimer>
#include <QMessageBox>
#include "quizsummarywidget.h"

SingleQuizWidget::SingleQuizWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleQuizWidget)
  , questionWidget(new BasicQuestionWidget(this))
  , summaryWidget(new QuizSummaryWidget)
  , shortTimer(new QTimer(this))
  , longTimer(new QTimer(this))
{
    ui->setupUi(this);
   // setQuiz(quiz);
    ui->stackedWidget->addWidget(questionWidget);
    ui->stackedWidget->addWidget(summaryWidget);


    connect(summaryWidget, &QuizSummaryWidget::confirmed, this, &SingleQuizWidget::quizFinished);
    connect(questionWidget, &BasicQuestionWidget::answerConfirmed, this, &SingleQuizWidget::onAnswerConfirmed);
    connect(this, &SingleQuizWidget::quizCompleted, this, &SingleQuizWidget::onQuizCompleted);
    connect(ui->endQuizButton, &QPushButton::clicked, this, &SingleQuizWidget::onEndButtonClicked);
    connect(this, &SingleQuizWidget::timeout, this, &SingleQuizWidget::onTimeout);
}

SingleQuizWidget::SingleQuizWidget(singleQuizPtr quiz, QWidget *parent)
    :SingleQuizWidget(parent)
{
    setQuiz(quiz);
}

SingleQuizWidget::~SingleQuizWidget()
{
    delete ui;
}

void SingleQuizWidget::setQuiz(singleQuizPtr quiz)
{
    this->quiz = quiz;
    if(quiz.isNull()) return;
    setupGui();
}

void SingleQuizWidget::setupGui()
{
    totalPoints = 0;
    currentQuestion = 0;

    if(quiz->isTimeSpecified()){
        this->timeCounter = true;
        ui->timeWidget->show();
    }
    else{
        this->timeCounter = false;
        ui->timeWidget->hide();
    }
    ui->endQuizButton->show();
    ui->titleLabel->setText(quiz->getTitle());
    this->questionWidget->startTime();
    if(quiz->getQuestions().size() == 0){
        emit this->quizCompleted(0, countEndTime());
        return;
    }
    questionWidget->setQuestion(quiz->getQuestions().at(0));
    ui->stackedWidget->setCurrentWidget(questionWidget);
}

void SingleQuizWidget::startTimer()
{
    const static int interval = 10; //in milliseconds
    const static int barMaxValue = 10000;
    shortTimer->disconnect();
    longTimer->disconnect();
    startTime = QTime::currentTime();
    if(timeCounter){
        //hide spacers

        ui->timeWidget->show();
        this->setSpacerVisibility(false);

        ui->timeProgBar->setMaximum(barMaxValue);
        ui->timeProgBar->setValue(barMaxValue);
        shortTimer->setInterval(interval);

        const int maxTime = quiz->getMaxTime().msecsSinceStartOfDay();
        int currentTime = maxTime;

        auto lambda = [=]()mutable{
            currentTime -= interval;
            ui->timeProgBar->setValue((static_cast<double>(longTimer->remainingTime())/maxTime) * barMaxValue);
            if(ui->timeProgBar->value() <= 0){
                shortTimer->disconnect();
            }
        };

        connect(shortTimer, &QTimer::timeout, this, lambda);

        longTimer->setInterval(maxTime);
        connect(longTimer, &QTimer::timeout, this, &SingleQuizWidget::timeout);
        connect(this, &SingleQuizWidget::quizCompleted, longTimer, &QTimer::stop);

        longTimer->start();
        shortTimer->start();
    }
}

void SingleQuizWidget::onAnswerConfirmed(int points)
{
    totalPoints +=points;
    if(currentQuestion < this->quiz->getQuestions().size() - 1){
        currentQuestion++;
        this->questionWidget->setQuestion(this->quiz->getQuestions().at(currentQuestion));
        this->questionWidget->startTime();
    }
    else{
        emit this->quizCompleted(totalPoints, countEndTime());
    }
}

void SingleQuizWidget::onQuizCompleted(int points, QTime time)
{
    this->stopTimers();

    ui->stackedWidget->setCurrentWidget(summaryWidget);
    ui->endQuizButton->hide();
    summaryWidget->setQuiz(this->quiz);
    summaryWidget->setPoints(points);
    summaryWidget->setTime(time);
}

void SingleQuizWidget::onEndButtonClicked()
{
   QMessageBox::StandardButton choice = QMessageBox::information(this,tr("Zakończyć quiz?"),tr("Jeśli zakończysz quiz, stracisz wszystkie postępy"),
                            QMessageBox::StandardButton::Yes | QMessageBox::Cancel);
   if(choice == QMessageBox::StandardButton::Yes){
        stopTimers();
        emit this->quizFinished();
   }

}

void SingleQuizWidget::onTimeout()
{
    stopTimers();
    emit this->quizCompleted(totalPoints, countEndTime());
    QMessageBox::information(this, tr("Czas się skończył"), tr("Koniec czasu, kliknij OK"));
}

void SingleQuizWidget::stopTimers()
{
    shortTimer->stop();
    longTimer->stop();
    this->ui->timeWidget->hide();
    setSpacerVisibility(true);
    this->questionWidget->stopTimers();
}

void SingleQuizWidget::setSpacerVisibility(bool value)
{
    ui->spacerWidget->setVisible(value);
}

QTime SingleQuizWidget::countEndTime()
{
    return QTime::fromMSecsSinceStartOfDay(QTime::currentTime().msecsSinceStartOfDay() - startTime.msecsSinceStartOfDay());
}
