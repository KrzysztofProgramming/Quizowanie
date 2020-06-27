#ifndef MULTIQUIZWIDGET_H
#define MULTIQUIZWIDGET_H
#include <QStackedWidget>
#include "./logic/singlequiz.h"

class QuizAreaDisplay;
class QuizRepresentationWidget;
class SingleQuizWidget;

class MultiQuizWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MultiQuizWidget(QWidget *parent = nullptr);

signals:
    void backToMenu();
public:

    inline const QList<singleQuizPtr>& getQuizes() const {return quizes;}
    void setQuiz(const QList<singleQuizPtr>& list);
    void addQuiz(const singleQuizPtr& quiz);
    void removeQuiz(const singleQuizPtr& quiz);

    QuizAreaDisplay *getDisplay() const {return display;}

private slots:
    void onQuizStart(singleQuizPtr quiz);

private:
    QuizAreaDisplay *display;
    QuizRepresentationWidget *representationWidget;
    SingleQuizWidget *quizWidget;
    QList<singleQuizPtr> quizes;

    void setupGui();
};

#endif // MULTIQUIZWIDGET_H
