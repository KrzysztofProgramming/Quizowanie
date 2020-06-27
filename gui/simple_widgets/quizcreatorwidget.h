#ifndef QUIZCREATORWIDGET_H
#define QUIZCREATORWIDGET_H

#include <QStackedWidget>
#include <QFuture>
#include "./logic/basicquestion.h"
#include "./logic/singlequiz.h"

QT_BEGIN_NAMESPACE
class CreateQuizWidget;
class CreateQuestionWidget;
class QuestionListElement;
class MainWindow;
class LoadingWidget;
class BusyLabel;

template<typename T>
class QFutureWatcher;
QT_END_NAMESPACE

class QuizCreatorWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit QuizCreatorWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    singleQuizPtr getQuiz() const;
    void setQuiz(const singleQuizPtr& quiz);
    inline void setQuizToEdit(const singleQuizPtr& quiz) { setQuiz(quiz); this->quizToEdit = quiz;}

    void reset();

signals:
    void back(bool fromEditMode);
    void quizSaved(singleQuizPtr quiz);
    void quizEdited(singleQuizPtr oldQuiz, singleQuizPtr newQuiz);

private slots:
    void onAddingQuestion();
    void onNewQuestionDenied();
    void onConfirmNewQuestion(basicQuestionPtr question);
    void onEditingQuestion(QuestionListElement *element);
    void onQuizConfirmed(singleQuizPtr quiz);
    void onWatcherFinished();

private:
    CreateQuizWidget *quizCreator;
    CreateQuestionWidget *questionCreator;
    QuestionListElement *editingElement;
    BusyLabel *busyLabel;
    QFutureWatcher<bool> *watcher;
    QFuture<bool> future;


    singleQuizPtr quizToEdit = nullptr;
    MainWindow *window;

    singleQuizPtr quizFromConfirmed = nullptr;
    bool good = true;

};

#endif // QUIZCREATORWIDGET_H
