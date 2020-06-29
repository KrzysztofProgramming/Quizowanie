#ifndef CREATEQUIZWIDGET_H
#define CREATEQUIZWIDGET_H

#include <QWidget>
#include "./logic/basicquestion.h"
#include "./logic/singlequiz.h"

namespace Ui {
class CreateQuizWidget;
}

QT_BEGIN_NAMESPACE
class PixmapLabel;
class QuestionListWidget;
class ImageDialog;
class QuestionListElement;
QT_END_NAMESPACE

class CreateQuizWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateQuizWidget(QWidget *parent = nullptr);
    ~CreateQuizWidget();

    void addQuestion(const basicQuestionPtr& question);
    bool isValidQuiz() const;
    singleQuizPtr getQuiz() const;
    void setQuiz(const singleQuizPtr& quiz);

    const QStringList& getForbiddenTitltes() const {return forbiddenTitles;}
    void setForbiddenTitles(const QStringList& list) {this->forbiddenTitles = list;}
    void setForbiddenTitles(QStringList&& list) {this->forbiddenTitles = list;}

    QStringList& getForbiddenTitles() {return forbiddenTitles;}
    const QStringList& getForbiddenTitles() const {return forbiddenTitles;}

    bool isEmpty() const;
    inline bool hasImageChanged() const {return imageChanged;}

public slots:
    void reset();

signals:
    void addNewQuestionClicked();
    void startEditQuestion(QuestionListElement *question);
    void quizConfirm(singleQuizPtr quiz);
    void quizDeny();

private slots:
    void onSelectImageClicked();
    void onConfirmButtonClicked();
    void onDenyButtonClicked();
    void onQuizDenied();


private:
    Ui::CreateQuizWidget *ui;
    PixmapLabel *imgLabel;
    QuestionListWidget *questionList;
    ImageDialog *dialog;

    bool imageChanged = false;
    //quiz set form setQuiz();
    singleQuizPtr quiz=nullptr;
    /**
     * @brief forbiddenTitles blocks signal quizConfirm if quiz has forbiddenTitle, but quiz is still valid
     * @see getDorbiddenTitles(), setForbiddenTitles()
     */
    QStringList forbiddenTitles;

    bool validateTitle(const QString& title);

    static constexpr int SIZE =2;
    const QString forbiddenAloneSymbols = "\t ";
};
#endif // CREATEQUIZWIDGET_H
