#include "createquizwidget.h"
#include "ui_createquizwidget.h"
#include "./gui/simple_widgets/pixmaplabel.h"
#include "./gui/question_widgets/questionlistwidget.h"
#include "./gui/question_widgets/questionlistelement.h"
#include "./gui/simple_widgets/imagedialog.h"
#include "./logic/database/databasemanager.h"
#include <QMessageBox>

CreateQuizWidget::CreateQuizWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateQuizWidget)
  , imgLabel(new PixmapLabel(this))
  , questionList(new QuestionListWidget(this))
  , dialog(new ImageDialog(this))
{
    ui->setupUi(this);
    this->ui->imageLayout->insertWidget(0, imgLabel);
    imgLabel->setText(tr("Nie wybrano obrazu"));
    dialog->setDescription(tr("Obraz będzie wyświetlany wraz z tytułem quizu, pełni jedynie role dekoracyjne"));

    this->ui->questionsScrollArea->setWidget(questionList);
    this->ui->questionsScrollArea->setWidgetResizable(true);

    connect(questionList->addButton(), &QPushButton::clicked, this, &CreateQuizWidget::addNewQuestionClicked);
    connect(questionList, &QuestionListWidget::startQuestionEdit, this, &CreateQuizWidget::startEditQuestion);

    connect(this->ui->imageButton, &QPushButton::clicked, this, &CreateQuizWidget::onSelectImageClicked);
    connect(this->ui->timeCheckBox, &QCheckBox::stateChanged, this, [=](bool value){
        this->ui->timeEdit->setEnabled(value);
    });

    connect(this->ui->confirmButton, &QPushButton::clicked, this, &CreateQuizWidget::onConfirmButtonClicked);
    connect(this->ui->denyButton, &QPushButton::clicked, this, &CreateQuizWidget::onDenyButtonClicked);
    connect(this, &CreateQuizWidget::quizDeny, this, &CreateQuizWidget::onQuizDenied);

}

CreateQuizWidget::~CreateQuizWidget()
{
    delete ui;
}

void CreateQuizWidget::addQuestion(const basicQuestionPtr &question)
{
    this->questionList->addQuestion(question);
}

bool CreateQuizWidget::isValidQuiz() const
{
    return this->questionList->getValidQuestionCount() > 0 && !this->ui->titleEdit->text().isEmpty();
}

singleQuizPtr CreateQuizWidget::getQuiz() const
{
    if(!isValidQuiz()) return nullptr;

    singleQuizPtr quiz(new SingleQuiz());
    quiz->setTitle(this->ui->titleEdit->text());
    if(this->ui->timeCheckBox->isChecked()){
        quiz->setMaxTime(this->ui->timeEdit->time());
    }
    quiz->setTitleImage(this->imgLabel->getPixmap());
    quiz->setQuestions(this->questionList->getQuestions());

    return quiz;
}

void CreateQuizWidget::setQuiz(const singleQuizPtr& quiz)
{
    const QTime& time = quiz->getMaxTime();
    const QList<basicQuestionPtr>& list = quiz->getQuestions();
    const QPixmap& image = quiz->getTitleImage();
    const QString& title = quiz->getTitle();

    this->ui->titleEdit->setText(title);
    this->imgLabel->setPixmap(image);;
    if(!time.isNull()){
        this->ui->timeCheckBox->setChecked(true);
        this->ui->timeEdit->setTime(time);
    }
    else{
        this->ui->timeCheckBox->setChecked(false);
    }
    this->questionList->setQuestions(list);

    this->quiz = quiz;
    this->imageChanged = false;
}

bool CreateQuizWidget::isEmpty() const
{
    if(!this->ui->titleEdit->text().isEmpty()) return false;
    if(this->ui->timeCheckBox->isChecked()) return false;
    if(this->questionList->getValidQuestionCount() > 0 ) return false;
    if(!this->imgLabel->getPixmap().isNull()) return false;

    return true;
}

void CreateQuizWidget::reset()
{
    this->ui->timeCheckBox->setChecked(false);
    this->ui->titleEdit->clear();
    this->questionList->removeAllQuestions();
    this->imgLabel->setPixmap(QPixmap());
    this->quiz = nullptr;
    this->imageChanged = false;
}

void CreateQuizWidget::onSelectImageClicked()
{
    dialog->setPixmap(this->imgLabel->getPixmap());
    int value = dialog->exec();
    if(value == QDialog::Accepted){
        this->imgLabel->setPixmap(dialog->getPixmap());
        this->imageChanged = true;
    }
}

void CreateQuizWidget::onConfirmButtonClicked()
{
    QString message("");

    if(!(this->questionList->getValidQuestionCount() > 0)){
        message = tr("Quiz musi zawierać co najmniej 1 pytanie!");
    }
    else if(this->ui->titleEdit->text().isEmpty() || !this->validateTitle(this->ui->titleEdit->text())){
        message = tr("Wprowadź tytuł quizu!");
    }
    else if(this->forbiddenTitles.contains(this->ui->titleEdit->text())){
        if(this->quiz==nullptr){
            message = tr("Nie możesz stworzyć tego quizu, zmień jego nazwę");
        }
        else if(this->quiz->getTitle() != this->ui->titleEdit->text()){
            message = tr("Wprowadź inny tutył");
        }
    }

    if(message.isEmpty()){
        emit this->quizConfirm(this->getQuiz());
        return;
    }

    QMessageBox::warning(this, "Zły quiz", message);

}

void CreateQuizWidget::onDenyButtonClicked()
{
    if((!this->quiz.isNull() && (imageChanged || *(this->quiz.get())!=*(this->getQuiz().get())))||
            (this->quiz.isNull() && !this->isEmpty())){
        int value = QMessageBox::warning(this, tr("Niezapisano"), tr("Czy na pewno chcesz wyjść bez zapisu?"),
                                         QMessageBox::Yes | QMessageBox::Cancel);
        if(value == QMessageBox::Yes){
            emit this->quizDeny();
        }
    }
    else emit this->quizDeny();
}

void CreateQuizWidget::onQuizDenied()
{
    this->quiz = nullptr;
    imageChanged = false;
}

bool CreateQuizWidget::validateTitle(const QString &title)
{
    for(const QChar& c: qAsConst(title)){
        if(!this->forbiddenAloneSymbols.contains(c)) return true;
    }
    return false;
}
