#include "questionlistelement.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

QuestionListElement::QuestionListElement(QWidget *parent) : QWidget(parent)
  ,layout(new QHBoxLayout)
  ,titleLabel(new QLabel(this))
  ,numberLabel(new QLabel(this))
  ,_editButton(new QPushButton(tr("edytuj"), this))
  ,_removeButton(new QPushButton(tr("usuń"), this))
  , upButton(new QPushButton(this))
  , downButton(new QPushButton(this))
{
    static QIcon upIcon(":/images/img/up.png");
    static QIcon downIcon(":/images/img/down.png");

    this->setLayout(layout);

    layout->addWidget(numberLabel);
    layout->addWidget(upButton);
    layout->addWidget(downButton);
    layout->addWidget(titleLabel);
    layout->addWidget(_editButton);
    layout->addWidget(_removeButton);

    this->upButton->setIcon(upIcon);
    this->downButton->setIcon(downIcon);
    this->titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->titleLabel->setMinimumSize(1,1);

    this->buttons = NoButton;
    this->upButton->hide();
    this->downButton->hide();
    this->numberLabel->hide();

    connect(this, &QuestionListElement::buttonsChanged, this, &QuestionListElement::onButtonsChanged);
}

void QuestionListElement::setQuestion(const basicQuestionPtr& question)
{
    this->question = question;
    reloadGui();
}

void QuestionListElement::setNumber(int number)
{
    if(number < 0){
        this->numberLabel->hide();
        this->number = -1;
        return;
    }
    else{
        this->numberLabel->show();
        this->number = number;
        this->numberLabel->setText(QString::number(number) + ".");
    }

}

void QuestionListElement::onButtonsChanged()
{
    if(this->buttons & UpButton) this->upButton->show();
    else this->upButton->hide();

    if(this->buttons & DownButton) this->downButton->show();
    else this->downButton->hide();
}


void QuestionListElement::reloadGui()
{
    QString copy = question->getDescription();
    this->titleLabel->setText(copy.replace('\n', ' '));
}
