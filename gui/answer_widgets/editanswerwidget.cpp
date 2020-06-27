#include "editanswerwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QCheckBox>
#include <QMouseEvent>
#include "./gui/answer_widgets/answerslistwidget.h"

EditAnswerWidget::EditAnswerWidget(AnswersListWidget *parent) : QFrame(parent)
  , saveEditButton(new QPushButton(this))
  , _removeButton(new QPushButton(tr("usuń"),this))
  , answerLabel(new QLabel(this))
  , answerEdit(new QLineEdit(this))
  , layout(new QHBoxLayout)
  , numberLabel(new QLabel(this))
  , isCorrectCheckBox(new QCheckBox(tr("Poprawna"), this))
  , parentList(parent)
  , state(Display) //by default this wiget is in Edit state, because it calls switchState() then;
{
    this->setLayout(this->layout);

    layout->addWidget(numberLabel);
    layout->addWidget(answerLabel);
    layout->addWidget(answerEdit);
    layout->addWidget(isCorrectCheckBox);
    layout->addWidget(saveEditButton);
    layout->addWidget(_removeButton);

    numberLabel->hide();
    answerLabel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    answerLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    answerEdit->setAlignment(Qt::AlignmentFlag::AlignCenter);
    this->setMaxAnswerSize(90);
    this->setAnswer("");
    this->answerEdit->setPlaceholderText(tr("odpowiedź"));

    switchState();

    connect(this->saveEditButton, &QPushButton::clicked, this, &EditAnswerWidget::switchState);
    connect(this, &EditAnswerWidget::answerValidationChange, this, &EditAnswerWidget::onAnswerValidationChange);
    connect(this->isCorrectCheckBox, &QCheckBox::stateChanged, this, &EditAnswerWidget::answerCorrectionChange);
}

void EditAnswerWidget::setMaxAnswerSize(int size)
{
    answerEdit->setMaxLength(size);
}

int EditAnswerWidget::getMaxAnswerSize() const
{
    return answerEdit->maxLength();
}

QString EditAnswerWidget::getAnswer() const
{
    if(state == Edit) return "";
    return this->answerLabel->text();
}

void EditAnswerWidget::setAnswer(const QString &answer, bool isCorrect)
{
    this->answerLabel->setText(answer);
    this->answerEdit->setText(answer);
    this->isCorrectCheckBox->setChecked(isCorrect);
}

void EditAnswerWidget::setNumber(int number)
{
    if(number < 0){
        this->numberLabel->hide();
        this->number = -1;
    }
    else{
        this->numberLabel->show();
        this->numberLabel->setText(QString::number(number) + '.');
        this->number = number;
    }
}

int EditAnswerWidget::getNumber() const
{
    return number;
}

bool EditAnswerWidget::isCorrect() const
{
    return this->isCorrectCheckBox->isChecked();
}

void EditAnswerWidget::switchState()
{
    switch(this->state){
    case Edit:{ //changing to display state
        this->saveEditButton->setText(tr("edytuj"));
        this->answerLabel->setText(answerEdit->text());

        this->answerEdit->hide();
        this->answerLabel->show();

        this->state = Display;
        emit stateChanged(Display);
        break;
    }
    case Display:{ //changing to Edit state
        this->saveEditButton->setText(tr("zapisz"));
        this->answerEdit->setText(answerLabel->text());

        this->answerEdit->show();
        this->answerLabel->hide();
        this->answerEdit->setFocus();

        this->state = Edit;
        emit stateChanged(Edit);
        break;
    }
    }
}

void EditAnswerWidget::onAnswerValidationChange(bool value)
{
    this->saveEditButton->setEnabled(value);
}


void EditAnswerWidget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    if(event->key() == Qt::Key::Key_Return){
        if(isValid()){
            this->setState(Display);
        }
    }
}


void EditAnswerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
    if(this->answerLabel->geometry().contains(event->pos())){
        if(isValid()){
            this->setState(Edit);
        }
    }
}
