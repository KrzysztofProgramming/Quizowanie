#ifndef QUIZAREADISPLAY_H
#define QUIZAREADISPLAY_H

#include <QWidget>
#include "./logic/singlequiz.h"
#include "./gui/quiz_widgets/quiziconwidget.h"

QT_BEGIN_NAMESPACE
class FlowLayout;
class QScrollArea;
class QPushButton;
class QuizIconWidget;
QT_END_NAMESPACE

class QuizAreaDisplay : public QWidget
{
    Q_OBJECT
public:
    constexpr static int columnCount = 3;

    explicit QuizAreaDisplay(QWidget *parent = nullptr);

    void addQuiz(const singleQuizPtr& quiz);
    /**
     * @brief removeQuiz remove icon with this quiz and also delete this icon
     * @param quiz
     */
    void removeQuiz(const singleQuizPtr& quiz);

    inline QPushButton* returnButton() const {return retButton;}
    void clearAndRemoveAllIcons();
    void setIconsMode(QuizIconWidget::Mode mode);

    inline QuizIconWidget::Mode getCurrentMode() const {return currentMode;}

private:
    FlowLayout *mainLayout;
    QScrollArea *scrollArea;
    QPushButton *retButton;
    QList<QuizIconWidget*> widgets;
    QuizIconWidget::Mode currentMode = QuizIconWidget::Display;

    void addQuizIcon(QuizIconWidget* widget);
    void removeQuizIcon(QuizIconWidget* widget);

signals:
    void startQuizClicked(singleQuizPtr quiuz);
    void editQuizClicked(singleQuizPtr quiz);
    void removeQuizClicked(singleQuizPtr quiz);

    // QWidget interface
public:
    virtual QSize minimumSizeHint() const override;
};

#endif // QUIZAREADISPLAY_H
