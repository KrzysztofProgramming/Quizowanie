#ifndef QUESTIONLISTELEMENT_H
#define QUESTIONLISTELEMENT_H

#include <QWidget>
#include "./logic/basicquestion.h"

QT_BEGIN_NAMESPACE
class QuestionListWidget;
class QHBoxLayout;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class QuestionListElement : public QWidget
{
    Q_OBJECT

    friend class QuestionListWidget;

public:
    enum Button{
        NoButton = 0,
        UpButton = 1,
        DownButton = 2,
        AllButtons = 3
    };
    Q_DECLARE_FLAGS(Buttons, Button);
    Q_FLAG(Buttons);

    explicit QuestionListElement(QWidget *parent = nullptr);

    inline QPushButton* removeButton() const {return _removeButton;}
    inline QPushButton* editButton() const {return _editButton;}
    inline QPushButton* getUpButton() const {return this->upButton;}
    inline QPushButton* getDownButton() const {return this->downButton;}
    inline const basicQuestionPtr& getQuestion() const {return question;}

    void setQuestion(const basicQuestionPtr& question);

    inline int getNumber() const {return number;}
    void setNumber(int number);
    void setButtons(Buttons buttons) {if(this->buttons!=buttons) {this->buttons = buttons, emit buttonsChanged();}}
    Buttons getButtons() const {return buttons;}

signals:
    void buttonsChanged();

private slots:
    void onButtonsChanged();

private:
    QHBoxLayout *layout;
    QLabel *titleLabel;
    QLabel *numberLabel;
    QPushButton *_editButton;
    QPushButton *_removeButton;
    QPushButton *upButton;
    QPushButton *downButton;
    Buttons buttons;

    basicQuestionPtr question;
    int number = -1;

    void reloadGui();

};

#endif // QUESTIONLISTELEMENT_H
