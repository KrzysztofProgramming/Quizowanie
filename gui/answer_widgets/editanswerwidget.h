#ifndef EDITANSWERWIDGET_H
#define EDITANSWERWIDGET_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QLineEdit;
class QHBoxLayout;
class QCheckBox;
class AnswersListWidget;
QT_END_NAMESPACE

class EditAnswerWidget : public QFrame
{
    Q_OBJECT
    friend class AnswersListWidget;

public:
    enum State {Display = 0, Edit = 1};
    explicit EditAnswerWidget(AnswersListWidget *parent);

    inline QPushButton* removeButton() const {return _removeButton;}
    inline QLineEdit* getAnswerEdit() const {return answerEdit;}

    void setMaxAnswerSize(int size);
    int getMaxAnswerSize() const;
    /**
     * @brief getAnswer
     * @return answer or empty string if widget is in edit mode;
     */
    QString getAnswer() const;

    State getState() const {return state;}
    void setAnswer(const QString& answer, bool isCorrect = false);

    void setNumber(int number);
    int getNumber() const;

    inline void setState(State newState){if(state != newState) {switchState();} }
    /**
     * @brief isValid validation describs saveEditButto enabled;
     * @return
     */
    inline bool isValid() const {return validAnswer;}

    bool isCorrect() const;

signals:
    void stateChanged(State newState);
    void answerValidationChange(bool value);
    void answerCorrectionChange(bool value);

private slots:
    void onAnswerValidationChange(bool value);
    void switchState();

private:
    QPushButton *saveEditButton;
    QPushButton *_removeButton;
    QLabel *answerLabel;
    QLineEdit *answerEdit;
    QHBoxLayout *layout;
    QLabel *numberLabel;
    QCheckBox *isCorrectCheckBox;
    AnswersListWidget *parentList = nullptr;
    State state;
    bool validAnswer = true;
    int number = -1;

    inline void setAnswerValidation(bool value) {if(value!=validAnswer){this->validAnswer = value, emit answerValidationChange(value);}}

    // QWidget interface
protected:
    //save changed when enter pressed
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // EDITANSWERWIDGET_H
