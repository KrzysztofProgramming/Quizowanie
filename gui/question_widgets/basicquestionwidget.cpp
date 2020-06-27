#include "basicquestionwidget.h"
#include "ui_questionwidget.h"
#include <QTimer>
#include "./logic/singlequiz.h"
#include "./logic/multiquestion.h"
#include "./logic/singlequestion.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QDebug>
#include "./gui/simple_widgets/pixmaplabel.h"

BasicQuestionWidget::BasicQuestionWidget(QWidget *parent) : QWidget(parent)
  , ui(new Ui::QuestionWidget)
  , shortTimer(new QTimer(this))
  , longTimer(new QTimer(this))
  , imgLabel(new PixmapLabel(this))
{
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout();
    this->ui->answerBox->setLayout(layout);
    imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imgLabel->hide();
    this->ui->imageLabelLayout->addWidget(imgLabel);

    connect(this, &BasicQuestionWidget::timeout, this, &BasicQuestionWidget::onTimeout);
    connect(ui->nextButton, &QPushButton::clicked, this, &BasicQuestionWidget::onNextButtonClicked);
    connect(this, &BasicQuestionWidget::answerConfirmed, this, &BasicQuestionWidget::onAnswerConfirmed);
}

BasicQuestionWidget::~BasicQuestionWidget()
{
    delete ui;
}

void BasicQuestionWidget::setQuestion(basicQuestionPtr question)
{
    this->question = question;
    setupGui();
}
bool BasicQuestionWidget::isMultiQuestion()
{
    return dynamic_cast<MultiQuestion*>(this->question.get()) != nullptr;
}

void BasicQuestionWidget::stopTimers()
{
    shortTimer->stop();
    longTimer->stop();
}

void BasicQuestionWidget::onTimeout()
{
    QMessageBox::warning(this, tr("Koniec czasu!"), tr("Kliknij OK, aby przejść do kolejnego pytania"));
    onNextButtonClicked();
}

void BasicQuestionWidget::onNextButtonClicked()
{
    if(isMultiQuestion()){
        const QList<int>& goodAnswers = static_cast<MultiQuestion*>(question.get())->getGoodAnswersIndexes();
        for(int x: goodAnswers){
            if(!buttons.at(x)->isChecked()){
                emit this->answerConfirmed(0);
                return;
            }
        }

        for(int i=0 ;i<buttons.size(); i++){
            if(buttons.at(i)->isChecked() && !goodAnswers.contains(i)){
                emit this->answerConfirmed(0);
                return;
            }
        }
        emit this->answerConfirmed(question->getPoints());
    }
    else{
        int goodAnswerIndex = static_cast<SingleQuestion*>(question.get())->getGoodAnswerIndex();
        for(int i=0; i<buttons.size(); i++){
            if(buttons.at(i)->isChecked()){
                if(i == goodAnswerIndex){
                    emit this->answerConfirmed(question->getPoints());
                    return;
                }
                else{
                    emit this->answerConfirmed(0);
                    return;
                }
            }
        }
        emit this->answerConfirmed(0);
    }
}

void BasicQuestionWidget::enableNextButton()
{
    ui->nextButton->setEnabled(true);
}

void BasicQuestionWidget::onAnswerConfirmed(int points)
{
    Q_UNUSED(points);
    stopTimers();
}

void BasicQuestionWidget::startTime()
{
    const static int interval = 20; //in milliseconds
    const static int barMaxValue = 10000;
    longTimer->stop();
    shortTimer->stop();
    shortTimer->disconnect();
    longTimer->disconnect();
    if(timeCounter){
        ui->timeWidget->show();
        ui->timeProgBar->setMaximum(barMaxValue);
        ui->timeProgBar->setValue(barMaxValue);
        shortTimer->setInterval(interval);

        const int maxTime = question->getTime().msecsSinceStartOfDay();
        int currentTime = maxTime;

        auto lambda = [=]()mutable{
            currentTime -= interval;
            ui->timeProgBar->setValue((static_cast<double>(longTimer->remainingTime())/maxTime) * barMaxValue);
            if(ui->timeProgBar->value() <= 0){
                shortTimer->disconnect();
                shortTimer->stop();
            }
        };

        connect(shortTimer, &QTimer::timeout, this, lambda);

        longTimer->setInterval(maxTime);

        connect(longTimer, &QTimer::timeout, this, &BasicQuestionWidget::timeout);

        longTimer->setSingleShot(true);
        longTimer->start(maxTime);
        shortTimer->start(interval);
    }

}

void BasicQuestionWidget::setupGui()
{
    const SingleQuiz * quiz = question->getQuiz();
    ui->timeWidget->hide();
    //time
    if(question->isTimeSpecified() && (quiz==nullptr || quiz->getMaxTime().isNull())){
        timeCounter = true;
    }
    else{
        timeCounter = false;
    }
    //image
    if(question->hasImage()){
        imgLabel->show();
        imgLabel->setPixmap(question->getImage());
    }
    else{
        imgLabel->hide();
    }
    //points and question number
    if(quiz == nullptr){
        ui->infoLabel->setText(QString::number(question->getPoints()) + " pkt.");
    }
    else{
       // qDebug() <<*quiz;
        ui->infoLabel->setText("Pytanie " + QString::number(quiz->getQuestions().indexOf(question) + 1) + "/" +
                               QString::number(quiz->getQuestions().size()) + " " + QString::number(question->getPoints()) + " pkt.");
    }

    //description
    this->ui->descriptionLabe->setText(question->getDescription());
    //next Button
    ui->nextButton->setEnabled(false);
    //buttons
    if(isMultiQuestion()){
        this->generateButtons<QCheckBox>();
        enableNextButton();
    }
    else{
        this->generateButtons<QRadioButton>();
        for(QAbstractButton* b: buttons){
            connect(b, &QAbstractButton::clicked, this, &BasicQuestionWidget::enableNextButton);
        }
    }
}
