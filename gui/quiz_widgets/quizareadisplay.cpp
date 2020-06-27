#include "quizareadisplay.h"
#include <QGridLayout>
#include <QEvent>
#include "./gui/others/flowlayout.h"
#include <QScrollBar>
#include <QScrollArea>
#include <QApplication>
#include <QStyle>
#include <QVBoxLayout>
#include <QPushButton>
#include "./gui/quiz_widgets/quiziconwidget.h"

QuizAreaDisplay::QuizAreaDisplay(QWidget *parent) : QWidget(parent)
  ,mainLayout(new FlowLayout)
  ,scrollArea(new QScrollArea)
  ,retButton(new QPushButton)
{
    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    mainLayout->setVerticalSpacing(10);
    mainLayout->setHorizontalSpacing(10);

    QVBoxLayout *layout = new QVBoxLayout;
    retButton->setText(tr("Powrót"));
    QFont font = retButton->font();
    font.setPointSize(10);
    retButton->setFont(font);
    layout->addWidget(this->scrollArea);
    layout->addWidget(retButton);

    this->setLayout(layout);

}

void QuizAreaDisplay::addQuizIcon(QuizIconWidget *widget)
{
    this->mainLayout->addWidget(widget);
    this->widgets.append(widget);

    connect(widget->startButton(), &QPushButton::clicked, this, [=](){
        emit this->startQuizClicked(widget->getQuiz());
    });
    connect(widget->editButton(), &QPushButton::clicked, this, [=](){
        emit this->editQuizClicked(widget->getQuiz());
    });
    connect(widget->removeButton(), &QPushButton::clicked, this, [=](){
        emit this->removeQuizClicked(widget->getQuiz());
    });
}

void QuizAreaDisplay::removeQuizIcon(QuizIconWidget *widget)
{
    this->widgets.removeAll(widget);
    mainLayout->removeWidget(widget);
    widget->deleteLater();
}

void QuizAreaDisplay::addQuiz(const singleQuizPtr &quiz)
{
    QuizIconWidget *widget = new QuizIconWidget(quiz, QSize(QuizIconWidget::defaultPrefferedWidth, QuizIconWidget::defaultPrefferedHeight), this);
    this->addQuizIcon(widget);
    widget->setMode(this->currentMode);
}

void QuizAreaDisplay::removeQuiz(const singleQuizPtr &quiz)
{
    for(int i=0; i<widgets.size(); i++){
        if(widgets.at(i)->getQuiz() == quiz){
            mainLayout->removeWidget(widgets.at(i));
            widgets.at(i)->deleteLater();
            this->widgets.removeAt(i);
        }
    }
}

void QuizAreaDisplay::clearAndRemoveAllIcons()
{
    for(QuizIconWidget* widget: widgets){
        mainLayout->removeWidget(widget);
        widget->deleteLater();
    }
    this->widgets.clear();
}

void QuizAreaDisplay::setIconsMode(QuizIconWidget::Mode mode)
{
    this->currentMode = mode;
    for(QuizIconWidget* widget: widgets){
        widget->setMode(mode);
    }
}

QSize QuizAreaDisplay::minimumSizeHint() const
{
    return mainLayout->minimumSize();// + QSize(QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent), 0);
}
