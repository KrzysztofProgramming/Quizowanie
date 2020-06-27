#ifndef CREATQUESTIONWIDGET_H
#define CREATQUESTIONWIDGET_H

#include <QWidget>
#include "./logic/basicquestion.h"


namespace Ui {
class CreatQuestionWidget;
}

QT_BEGIN_NAMESPACE
class AnswersListWidget;
class ImageDialog;
class EditAnswerWidget;
QT_END_NAMESPACE

class CreateQuestionWidget : public QWidget
{
    Q_OBJECT

public:
    static constexpr int MAX_DESCRIPTION_LENGTH = 999;

    explicit CreateQuestionWidget(QWidget *parent = nullptr);
    ~CreateQuestionWidget();

    bool isValidQuestion() const;
    basicQuestionPtr getQuestion() const;
    void setQuestion(basicQuestionPtr question);
    void reset();
    bool isEmpty() const;
    inline bool hasImageChanged() const {return imageChanged;}
signals:
    void timeSpecifiedChange(bool value);
    void questionConfirmed(basicQuestionPtr question);
    void questionDenied();

private slots:
    void onTimeSpecifiedChanged(bool value);
    void onConfirmClicked();
    void onChangeQuestionType();
    void openImageDialog();
    void onDenyClicked();
    void checkDescriptionLenght();
    void onEditingAnswerChanged(EditAnswerWidget *widget);

private:
    Ui::CreatQuestionWidget *ui;
    AnswersListWidget *answerList;
    ImageDialog *imgDialog;
    QPixmap image;

    //question set with setQuestion();
    basicQuestionPtr question = nullptr;\
    bool imageChanged = false;
    bool isTimeSpecified = false;

    inline void setTimeSpecified(bool value){if(value!=isTimeSpecified){ this->isTimeSpecified = value; emit timeSpecifiedChange(value);}}
};

#endif // CREATQUESTIONWIDGET_H
