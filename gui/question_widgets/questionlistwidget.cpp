#include "questionlistwidget.h"
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QPushButton>
#include "./gui/question_widgets/questionlistelement.h"

QuestionListWidget::QuestionListWidget(QWidget *parent) : QWidget(parent)
  ,layout(new QVBoxLayout)
  ,_addButton(new QPushButton(tr("Dodaj pytanie"), this))
  ,spacer(new QSpacerItem(0,0, QSizePolicy::Preferred, QSizePolicy::Expanding))
{
    this->setLayout(layout);

    this->layout->addItem(spacer);
    this->layout->addWidget(_addButton);

    connect(this, &QuestionListWidget::downButtonClicked, this, &QuestionListWidget::onDownButtonClicked);
    connect(this, &QuestionListWidget::upButtonClicked, this, &QuestionListWidget::onUpButtonClicked);
}

void QuestionListWidget::addQuestion(const basicQuestionPtr &question)
{
    QuestionListElement *element = new QuestionListElement(this);
    layout->insertWidget(layout->count() - 2, element);
    element->setQuestion(question);

    if(this->questionsList.size() >= 1){
        element->setButtons(QuestionListElement::UpButton);
        this->questionsList.back()->setButtons(this->questionsList.back()->getButtons() | QuestionListElement::DownButton);
    }

    this->questionsList.append(element);
    element->setNumber(this->questionsList.size());

    connect(element->removeButton(), &QPushButton::clicked, this, [=](){
        this->questionsList.removeAll(element);
        element->deleteLater();
        resetElementNumbers();
        emit this->questionRemoved(question);
    });

    connect(element->editButton(), &QPushButton::clicked, this, [=](){
        emit this->startQuestionEdit(element);
    });

    connect(element->upButton, &QPushButton::clicked, this, [=](){
        emit this->upButtonClicked(this->questionsList.indexOf(element));
    });
    connect(element->downButton, &QPushButton::clicked, this, [=](){
        emit this->downButtonClicked(this->questionsList.indexOf(element));
    });

}

void QuestionListWidget::setQuestions(const QList<basicQuestionPtr> &questions)
{
    removeAllQuestions();
    for(basicQuestionPtr q: questions)
        this->addQuestion(q);
}

QList<basicQuestionPtr> QuestionListWidget::getQuestions() const
{
    QList<basicQuestionPtr> list;
    for(auto element: this->questionsList){
        if(element->getQuestion().isNull() || !element->getQuestion()->isValid()) continue;
        list.append(element->getQuestion());
    }
    return list;
}

int QuestionListWidget::getValidQuestionCount() const
{
    int count = 0;
    for(auto element: this->questionsList){
        if(!element->getQuestion().isNull() && element->getQuestion()->isValid())
            ++count;
    }
    return count;
}

void QuestionListWidget::removeAllQuestions()
{
    for(QuestionListElement *widget: questionsList){
        layout->removeWidget(widget);
        widget->deleteLater();
    }
    this->questionsList.clear();
}

void QuestionListWidget::onUpButtonClicked(int elementIndex)
{
    if(elementIndex <= 0) return;
    QuestionListElement *element = this->questionsList.at(elementIndex);
    QuestionListElement *upElement = this->questionsList.at(elementIndex - 1);

    this->layout->removeWidget(element);

    this->layout->removeWidget(upElement);
    //kolejność ma znaczenie!
    this->layout->insertWidget(elementIndex - 1, element);
    this->layout->insertWidget(elementIndex, upElement);

    QuestionListElement::Buttons buttons = element->getButtons();
    element->setButtons(upElement->getButtons());
    upElement->setButtons(buttons);

    this->questionsList.swapItemsAt(elementIndex, elementIndex-1);

    resetElementNumbers();
}

void QuestionListWidget::onDownButtonClicked(int elementIndex)
{
    if(elementIndex >= this->questionsList.size()) return;
    QuestionListElement *element = this->questionsList.at(elementIndex);
    QuestionListElement *downElement = this->questionsList.at(elementIndex + 1);

    this->layout->removeWidget(element);
    this->layout->removeWidget(downElement);

    //kolejność ma znaczenie!
    this->layout->insertWidget(elementIndex, downElement);
    this->layout->insertWidget(elementIndex + 1, element);

    QuestionListElement::Buttons buttons = element->getButtons();
    element->setButtons(downElement->getButtons());
    downElement->setButtons(buttons);

    this->questionsList.swapItemsAt(elementIndex, elementIndex+1);

    resetElementNumbers();
}

void QuestionListWidget::resetElementNumbers()
{
    for(int i=0 ;i<questionsList.size(); i++){
        questionsList.at(i)->setNumber(i + 1);
    }
}
