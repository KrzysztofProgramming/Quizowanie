#include "quiziconwidget.h"
#include "ui_quiziconwidget.h"
#include "./gui/simple_widgets/pixmaplabel.h"

QuizIconWidget::QuizIconWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuizIconWidget)
  , imgLabel(new PixmapLabel(this))
{
    ui->setupUi(this);
    this->setFrameStyle(QFrame::Shape::Box);
    this->ui->mainLayout->insertWidget(1, imgLabel);
    this->imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->imgLabel->setText("");

    this->mode = Display;
    this->onModeChanged(this->mode);
    connect(this, &QuizIconWidget::modeChange, this, &QuizIconWidget::onModeChanged);
}

QuizIconWidget::QuizIconWidget(const singleQuizPtr &quiz, const QSize &size, QWidget *parent)
    :QuizIconWidget(parent)
{
    setQuiz(quiz);
    setPrefferedSize(size);
}

QuizIconWidget::~QuizIconWidget()
{
    delete ui;
}

void QuizIconWidget::setQuiz(const singleQuizPtr &quiz)
{
    this->quiz = quiz;
    refreshUI();
}

void QuizIconWidget::refreshUI()
{
    this->ui->titleLabel->setText(quiz->getTitle());
    this->ui->titleLabel->setWordWrap(true);
    if(quiz->hasImage()){
        this->imgLabel->show();
        this->imgLabel->setPixmap(quiz->getTitleImage());
    }
    else this->imgLabel->hide();
}

void QuizIconWidget::setPrefferedSize(const QSize &size)
{
    this->prefferedSize = size;
}

QPushButton *QuizIconWidget::startButton() const
{
    return this->ui->startButton;
}

QPushButton *QuizIconWidget::editButton() const
{
    return this->ui->editButton;
}

QPushButton *QuizIconWidget::removeButton() const
{
    return this->ui->removeButton;
}

void QuizIconWidget::onModeChanged(QuizIconWidget::Mode mode)
{
    switch (mode) {
    case Display:{
        this->ui->startButton->show();
        this->ui->editButton->hide();
        this->ui->removeButton->hide();
        break;
    }
    case Edit:{
        this->ui->startButton->hide();
        this->ui->editButton->show();
        this->ui->removeButton->show();
        break;
    }
    }
}

QSize QuizIconWidget::sizeHint() const
{
    return this->prefferedSize;
    if(this->prefferedSize.isValid()) return this->prefferedSize;
    else return QFrame::sizeHint();
}

QSize QuizIconWidget::minimumSizeHint() const
{
    return this->sizeHint();
}
