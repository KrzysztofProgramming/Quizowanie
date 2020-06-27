#ifndef SINGLEQUIZWIDGET_H
#define SINGLEQUIZWIDGET_H

#include <QWidget>
#include "./logic/singlequiz.h"

namespace Ui {
class SingleQuizWidget;
}
class BasicQuestionWidget;
class QuizSummaryWidget;

class SingleQuizWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingleQuizWidget(singleQuizPtr quiz, QWidget *parent = nullptr);
    explicit SingleQuizWidget(QWidget *parent = nullptr);
    ~SingleQuizWidget();

    void setQuiz(singleQuizPtr quiz);
    void setupGui();
    void startTimer();

signals:
    void quizCompleted(int points, QTime time);
    void quizFinished();
    void timeout();

private slots:
    void onAnswerConfirmed(int points);
    void onQuizCompleted(int points, QTime time);
    void onEndButtonClicked();
    void onTimeout();

private:
    Ui::SingleQuizWidget *ui;
    singleQuizPtr quiz = nullptr;

    int totalPoints = 0;
    bool timeCounter = false;
    int currentQuestion = 0;

    BasicQuestionWidget *questionWidget = nullptr;
    QuizSummaryWidget *summaryWidget = nullptr;
    QTimer *shortTimer = nullptr;
    QTimer *longTimer = nullptr;
    QTime startTime;

    void stopTimers();
    void setSpacerVisibility(bool value);
    QTime countEndTime();
};

#endif // SINGLEQUIZWIDGET_H
