#include "answerslistwidget.h"
#include "editanswerwidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include <QCheckBox>
#include <QDebug>
#include <QKeyEvent>

AnswersListWidget::AnswersListWidget(QWidget *parent) : QWidget(parent)
  ,layout(new QVBoxLayout)
  ,addButton(new QPushButton(tr("Dodaj odpowiedź"), this))
  ,spacer(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding))
{
    this->setLayout(this->layout);
    layout->addSpacerItem(spacer);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, this, &AnswersListWidget::createAnswerWidget);
}

QStringList AnswersListWidget::getAnswers() const
{
    QStringList list;
    for(auto widget: this->answerWidgets){
        if(widget->getState() == EditAnswerWidget::State::Display){
            list.append(widget->getAnswer());
        }
    }
    return list;
}

void AnswersListWidget::setAnswers(const QStringList& answers,const QList<int> &correctAnswers, bool isMulti)
{
    this->removeAllAnswerWidgets();
    this->setAcceptingMultiCorrectAnswer(isMulti);
    for(int i=0; i<answers.size(); i++){
        createAnswerWidget();
        this->answerWidgets.at(i)->setAnswer(answers.at(i), correctAnswers.contains(i));
        this->answerWidgets.at(i)->setState(EditAnswerWidget::Display);
    }
}

QList<int> AnswersListWidget::getCorrectAnswersIndexes() const
{
    QList<int> correctIndexes;
    for(int i=0; i<answerWidgets.size(); i++){
        EditAnswerWidget* widget = answerWidgets.at(i);
        if(widget->getState() == EditAnswerWidget::State::Display){
            if(widget->isCorrect()){
                correctIndexes.append(i);
            }
        }
    }
    return correctIndexes;
}

int AnswersListWidget::getValidAnswersCount() const
{
    int count = 0;
    for(const EditAnswerWidget* widget: this->answerWidgets){
        if(widget->isValid()) ++count;
    }
    return count;
}

void AnswersListWidget::setAcceptingMultiCorrectAnswer(bool value)
{
    if(value){
        for(EditAnswerWidget* widget: this->answerWidgets){
            widget->isCorrectCheckBox->setEnabled(true);
        }
    }
    else{
        int correctAnswersCount = this->getCorrectAnswersCount();
        if(correctAnswersCount == 1){
            for(EditAnswerWidget* widget: this->answerWidgets){
                if(!widget->isCorrect())
                    widget->isCorrectCheckBox->setEnabled(false);
            }
        }
        else{
            this->resetCorrectAnswers();
        }
    }
    this->acceptMultiCorrectAnswers = value;
}

int AnswersListWidget::getCorrectAnswersCount() const
{
    int count = 0;
    for(const EditAnswerWidget* widget: qAsConst(answerWidgets)){
        if(widget->isCorrect()) ++count;
    }
    return count;
}

void AnswersListWidget::createAnswerWidget()
{
    EditAnswerWidget *widget = new EditAnswerWidget(this);
    layout->removeWidget(addButton);
    layout->removeItem(spacer);

    layout->addWidget(widget);
    layout->addItem(spacer);
    layout->addWidget(addButton);

    this->answerWidgets.append(widget);
    widget->answerEdit->setFocus();
    widget->setNumber(this->answerWidgets.size());

    onStateChanged(widget);
    this->validateRepetitions(widget);
    this->validateAnswerCount(widget);

    connect(widget->removeButton(), &QPushButton::clicked, this, [=](){
        this->layout->removeWidget(widget);
        this->answerWidgets.removeAll(widget);
        if(widget->isCorrect() && !this->isAcceptingMultiCorrectAnswers()) this->resetCorrectAnswers();

        if(widget == currentEditingAnswer) setCurrentEditingAnswer(nullptr);
        this->validateRepetitions(currentEditingAnswer);

        widget->deleteLater();

        for(int i=0; i<this->answerWidgets.size(); i++){
            this->answerWidgets.at(i)->setNumber(i + 1);
        }

    });

    connect(widget, &EditAnswerWidget::stateChanged, this,[=](){
        this->onStateChanged(widget);
    });

    connect(widget, &EditAnswerWidget::answerCorrectionChange, this, [=](){
        this->onChangingCorrectAnswer(widget);
    });

    connect(widget->answerEdit, &QLineEdit::textChanged, this, [=](){
        this->validateRepetitions(widget);
    });
}

void AnswersListWidget::onStateChanged(EditAnswerWidget *changedWidget)
{
    if(changedWidget->getState() == EditAnswerWidget::Edit){
        if(getEditingAnswer()==nullptr){
            setCurrentEditingAnswer(changedWidget);
        }
        else{
            getEditingAnswer()->setState(EditAnswerWidget::Display);
            setCurrentEditingAnswer(changedWidget);
        }
    }
    else{
        if(getEditingAnswer() == changedWidget){
            setCurrentEditingAnswer(nullptr);
        }
    }
}

void AnswersListWidget::removeAllAnswerWidgets()
{
    for(EditAnswerWidget* widget: this->answerWidgets){
        layout->removeWidget(widget);
        widget->deleteLater();
    }
    this->answerWidgets.clear();
    this->addButton->setEnabled(true);

}

void AnswersListWidget::onChangingCorrectAnswer(EditAnswerWidget *widget)
{
    if(this->isAcceptingMultiCorrectAnswers()) return;

    int correctAnswers = this->getCorrectAnswersCount();

    if(correctAnswers == 1){
        for(EditAnswerWidget* x: this->answerWidgets){
            if(x!=widget){
                x->isCorrectCheckBox->setEnabled(false);
            }
        }
    }
    else if(correctAnswers == 0){
        for(EditAnswerWidget* x: this->answerWidgets){
            x->isCorrectCheckBox->setEnabled(true);
        }
    }

}

bool AnswersListWidget::validateRepetitions(EditAnswerWidget *widget)
{
    if(widget == nullptr){
        if(repetitionDetected){
            repetitionDetected = false;
            for(EditAnswerWidget *widget: answerWidgets){
                widget->setAnswerValidation(true);
                this->addButton->setEnabled(true);
            }
        }
        return true;
    }
    QString text = widget->answerEdit->text();
    QStringList list = this->getAnswers();
    if(text.isEmpty() || list.contains(text)){
        if(!repetitionDetected){
            repetitionDetected = true;
            for(EditAnswerWidget *widget: answerWidgets){
                widget->setAnswerValidation(false);
                this->addButton->setEnabled(false);
            }
        }
        return false;
    }
    else if(repetitionDetected){
        repetitionDetected = false;
        for(EditAnswerWidget *widget: answerWidgets){
            widget->setAnswerValidation(true);
            this->addButton->setEnabled(true);
        }
    }

    return true;
}

void AnswersListWidget::resetCorrectAnswers()
{
    for(EditAnswerWidget* widget: this->answerWidgets){
        widget->isCorrectCheckBox->setChecked(false);
        widget->isCorrectCheckBox->setEnabled(true);
    }
}

void AnswersListWidget::validateAnswerCount(EditAnswerWidget *widget)
{
    if(this->isAcceptingMultiCorrectAnswers()) return;

    if(this->getCorrectAnswersCount() >= 1) widget->isCorrectCheckBox->setEnabled(false);
}
