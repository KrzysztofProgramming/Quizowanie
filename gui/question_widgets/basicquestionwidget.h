#ifndef BASICQUESTIONWIDGET_H
#define BASICQUESTIONWIDGET_H

#include <QWidget>
#include "./logic/basicquestion.h"
#include <QAbstractButton>
#include <QVBoxLayout>
#include <ui_questionwidget.h>
#include <algorithm>
#include <random>

QT_BEGIN_NAMESPACE
class PixmapLabel;
QT_END_NAMESPACE

class BasicQuestionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BasicQuestionWidget(QWidget *parent = nullptr);
    virtual ~BasicQuestionWidget();

    void setQuestion(basicQuestionPtr question);
    basicQuestionPtr getQuestion() const {return question; }
    //void reset();
    void startTime();
    bool isMultiQuestion();
    void stopTimers();
signals:
    void timeout();
    void answerConfirmed(int points);

private slots:
    void onTimeout();
    void onNextButtonClicked();
    void enableNextButton();
    void onAnswerConfirmed(int points);

private:
    Ui::QuestionWidget *ui;

    QTimer *shortTimer = nullptr;
    QTimer *longTimer = nullptr;
    basicQuestionPtr question;
    QList<QAbstractButton*> buttons;
    PixmapLabel *imgLabel;

    bool timeCounter = false;

    void setupGui();



protected: //templates:

    template<typename T, typename std::enable_if<std::is_base_of<QAbstractButton, T>::value>::type* = nullptr>
    void generateButtons(){
        //groupBox
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->ui->answerBox->layout());
        if(layout==nullptr){
            return;
        }

        //clearing layout
        for(int i=0; i<buttons.size(); i++){
            layout->removeWidget(buttons.at(i));
            buttons.at(i)->deleteLater();
        }
        buttons.clear();
        QFont font;
        bool first = true;

        //generating random number positions
        QList<int> sequence;
        for(int i=0; i<question->getAnswers().size(); i++){
            sequence.append(i);
            T *button = new T(this->ui->answerBox);
            button->setText(question->getAnswers().at(sequence.at(i)));
            if(first){
                font = button->font();
                font.setPointSize(9);
                first = false;
            }
            button->setFont(font);
            buttons.append(button);
        }
        std::shuffle(sequence.begin(), sequence.end(), std::default_random_engine(QDateTime::currentMSecsSinceEpoch()));
        //generating buttons

        buttons.reserve(sequence.size());
        for(int i=0; i<question->getAnswers().size();i++){
            layout->addWidget(buttons.at(sequence.at(i)));
        }
    }
};

#endif // BASICQUESTIONWIDGET_H
