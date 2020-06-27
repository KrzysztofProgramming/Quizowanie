#ifndef QUIZREPRESENTATIONWIDGET_H
#define QUIZREPRESENTATIONWIDGET_H

#include <QWidget>
#include "logic/singlequiz.h"

namespace Ui {
class QuizRepresentationWidget;
}
QT_BEGIN_NAMESPACE
class QPushButton;
class PixmapLabel;
QT_END_NAMESPACE

class QuizRepresentationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuizRepresentationWidget(QWidget *parent = nullptr);
    ~QuizRepresentationWidget();
    void setQuiz(singleQuizPtr quiz);
    singleQuizPtr getQuiz() const {return quiz;}
    void refreshUI();
    QPushButton* returnButton() const;
    QPushButton *startButton() const;

private:
    Ui::QuizRepresentationWidget *ui;
    PixmapLabel *imgLabel;
    singleQuizPtr quiz;
};

#endif // QUIZREPRESENTATIONWIDGET_H
