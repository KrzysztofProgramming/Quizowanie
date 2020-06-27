#ifndef ANSWERSLISTWIDGET_H
#define ANSWERSLISTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class EditAnswerWidget;
class QVBoxLayout;
class QPushButton;
class QSpacerItem;
QT_END_NAMESPACE

class AnswersListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnswersListWidget(QWidget *parent = nullptr);

    QStringList getAnswers() const;
    void setAnswers(const QStringList& answers,const QList<int>& correctAnswers, bool isMulti = true);
    QList<int> getCorrectAnswersIndexes() const;
    int getCorrectAnswersCount() const;

    int getValidAnswersCount() const;
    bool isAcceptingMultiCorrectAnswers() const {return acceptMultiCorrectAnswers;}

    void removeAllAnswerWidgets();
    EditAnswerWidget* getEditingAnswer() const {return currentEditingAnswer;}

signals:
    void editingAnswerChanged(EditAnswerWidget *widget);

public slots:
    void setAcceptingMultiCorrectAnswer(bool value);

private slots:
    void createAnswerWidget();
    void onStateChanged(EditAnswerWidget *changedWidget);

private:
    QList<EditAnswerWidget*> answerWidgets;
    EditAnswerWidget* currentEditingAnswer = nullptr;
    QVBoxLayout *layout;
    QPushButton *addButton;
    QSpacerItem *spacer;

    inline void setCurrentEditingAnswer(EditAnswerWidget* widget) {if(currentEditingAnswer!=widget) {this->currentEditingAnswer = widget;
            emit editingAnswerChanged(widget);}}

    bool acceptMultiCorrectAnswers = false;

    /**
     * @brief validateRepetitions
     * @param widget
     * @return true if there were no repetitions
     */
    bool validateRepetitions(EditAnswerWidget* widget);
    void onChangingCorrectAnswer(EditAnswerWidget* widget);
    void resetCorrectAnswers();
    void validateAnswerCount(EditAnswerWidget* widget);

    bool repetitionDetected = false;

};

#endif // ANSWERSLISTWIDGET_H
