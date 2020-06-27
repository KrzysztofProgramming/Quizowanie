#ifndef QUESTIONLIST_H
#define QUESTIONLIST_H

#include <QWidget>
#include "./logic/basicquestion.h"

QT_BEGIN_NAMESPACE
class QuestionListElement;
class QVBoxLayout;
class QSpacerItem;
class QPushButton;
QT_END_NAMESPACE

class QuestionListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuestionListWidget(QWidget *parent = nullptr);
    void addQuestion(const basicQuestionPtr& question);
    void setQuestions(const QList<basicQuestionPtr>& questions);

    QPushButton* addButton() const {return _addButton;}
    /**
     * @brief getQuestions
     * @return valid questions list
     */
    QList<basicQuestionPtr> getQuestions() const;
    int getValidQuestionCount() const;
    void removeAllQuestions();

signals:
    void questionRemoved(basicQuestionPtr question);
    void startQuestionEdit(QuestionListElement *element);
    void upButtonClicked(int elementIndex);
    void downButtonClicked(int elementIndex);
private slots:
    void onUpButtonClicked(int elementIndex);
    void onDownButtonClicked(int elementIndex);

private:
    QList<QuestionListElement*> questionsList;

    QVBoxLayout *layout;
    QPushButton *_addButton;
    QSpacerItem *spacer;

    void resetElementNumbers();

};

#endif // QUESTIONLIST_H
