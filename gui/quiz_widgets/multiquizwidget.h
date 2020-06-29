#ifndef MULTIQUIZWIDGET_H
#define MULTIQUIZWIDGET_H
#include <QStackedWidget>
#include "./logic/singlequiz.h"

class QuizAreaDisplay;
class QuizRepresentationWidget;
class SingleQuizWidget;
class QuizStore;

/**
 * @brief The MultiQuizWidget class is a gui that allow to choose and play quiz
 */
class MultiQuizWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MultiQuizWidget(QWidget *parent, QuizStore* store);

signals:
    void backToMenu();
public:

    inline const QList<singleQuizPtr>& getQuizes() const {return quizes;}

    QuizAreaDisplay *getDisplay() const {return display;}

private slots:
    void onQuizStart(singleQuizPtr quiz);

    void setQuizes(const QList<singleQuizPtr>& list);
    void addQuiz(singleQuizPtr quiz);
    void removeQuiz(singleQuizPtr quiz);

private:
    QuizAreaDisplay *display;
    QuizRepresentationWidget *representationWidget;
    SingleQuizWidget *quizWidget;
    QList<singleQuizPtr> quizes;
    QuizStore *store;


    void setupGui();
};

#endif // MULTIQUIZWIDGET_H
