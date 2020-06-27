#ifndef QUIZSUMMARYWIDGET_H
#define QUIZSUMMARYWIDGET_H

#include <QWidget>
#include "./logic/singlequiz.h"
namespace Ui {
class QuizSummaryWidget;
}

QT_BEGIN_NAMESPACE
class PixmapLabel;
QT_END_NAMESPACE

class QuizSummaryWidget : public QWidget
{
    Q_OBJECT

public:
    const static int min2 = 30;
    const static int min3 = 50;
    const static int min3plus = 65;
    const static int min4 = 70;
    const static int min4plus = 85;
    const static int min5 = 90;
    const static int min6 = 100;
    static QString percentToGrade(double percent);

    explicit QuizSummaryWidget(singleQuizPtr quiz = nullptr, int pointsGot = 0, QWidget *parent = nullptr);
    ~QuizSummaryWidget();
    void setQuiz(singleQuizPtr quiz);
    void setPoints(int pointsGot);
    void setTime(const QTime& time);

signals:
    void confirmed();

private:
    Ui::QuizSummaryWidget *ui;
    singleQuizPtr quiz;
    PixmapLabel *imgLabel;
    int points;
    void setupGui();
};

#endif // QUIZSUMMARYWIDGET_H
