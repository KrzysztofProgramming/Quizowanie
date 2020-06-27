#include "createquestionwidget.h"
#include "ui_createquestionwidget.h"
#include "gui/answer_widgets/answerslistwidget.h"
#include "./logic/multiquestion.h"
#include "./logic/singlequestion.h"
#include "./gui/simple_widgets/imagedialog.h"
#include <QMessageBox>
#include "./gui/answer_widgets/editanswerwidget.h"
#include <QLineEdit>

CreateQuestionWidget::CreateQuestionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreatQuestionWidget)
  , answerList(new AnswersListWidget(this))
  , imgDialog(new ImageDialog(this))
{
    ui->setupUi(this);

    this->ui->answerScrollArea->setWidget(answerList);
    this->ui->answerScrollArea->setWidgetResizable(true);

    this->isTimeSpecified = this->ui->timeLimitButton->isChecked();
    this->onTimeSpecifiedChanged(this->isTimeSpecified);
    this->ui->questionTextEdit->setMaximumBlockCount(10);

    connect(this->ui->timeLimitButton, &QCheckBox::stateChanged, this, &CreateQuestionWidget::setTimeSpecified);
    connect(this, &CreateQuestionWidget::timeSpecifiedChange, this, &CreateQuestionWidget::onTimeSpecifiedChanged);
    connect(this->ui->singleQuestionButton, &QRadioButton::clicked, this, &CreateQuestionWidget::onChangeQuestionType);
    connect(this->ui->multiQuestionButton, &QRadioButton::clicked, this, &CreateQuestionWidget::onChangeQuestionType);
    connect(this->ui->imageButton, &QPushButton::clicked, this, &CreateQuestionWidget::openImageDialog);
    connect(this->ui->confirmButton, &QPushButton::clicked, this, &CreateQuestionWidget::onConfirmClicked);
    connect(this->ui->denyButton, &QPushButton::clicked, this, &CreateQuestionWidget::onDenyClicked);
    connect(this->ui->questionTextEdit, &QPlainTextEdit::textChanged, this, &CreateQuestionWidget::checkDescriptionLenght);
    connect(this->answerList, &AnswersListWidget::editingAnswerChanged, this, &CreateQuestionWidget::onEditingAnswerChanged);
}

CreateQuestionWidget::~CreateQuestionWidget()
{
    delete ui;
}

bool CreateQuestionWidget::isValidQuestion() const
{
    //at least 2 answers, and 1 correct if this is a single question
    return this->answerList->getAnswers().size() >= 2 &&
            (this->answerList->getCorrectAnswersCount() || this->ui->multiQuestionButton->isChecked()) &&
            !this->ui->questionTextEdit->toPlainText().isEmpty();
}

basicQuestionPtr CreateQuestionWidget::getQuestion() const
{
    if(!this->isValidQuestion()) return nullptr;
    basicQuestionPtr question;

    QStringList list = this->answerList->getAnswers();
    QList<int> correctAnswerList = this->answerList->getCorrectAnswersIndexes();
    QString questionDescription = this->ui->questionTextEdit->toPlainText();
    bool isMulti = this->ui->multiQuestionButton->isChecked();
    QTime time = (this->ui->timeLimitButton->isChecked()) ? this->ui->maxTimeEdit->time() : QTime();
    int points = this->ui->pointsSpinBox->value();

    if(isMulti){
        question = multiQuestionPtr(new MultiQuestion(questionDescription, list, correctAnswerList));
    }
    else{
        question = singleQuestionPtr(new SingleQuestion(questionDescription, list, correctAnswerList.at(0)));
    }
    question->setTime(time);
    question->setPoints(points);
    question->setImage(this->image);
    return question;

}

void CreateQuestionWidget::setQuestion(basicQuestionPtr question)
{
    const QTime& maxTime = question->getTime();
    int points = question->getPoints();
    QList<int> correctAnswers;
    const QStringList& answers = question->getAnswers();
    const QPixmap& pixmap = question->getImage();

    if(question->isMulti()){
        this->ui->multiQuestionButton->setChecked(true);
        correctAnswers = static_cast<MultiQuestion*>(question.get())->getGoodAnswersIndexes();
        this->answerList->setAnswers(answers, correctAnswers, true);
    }
    else{
        this->ui->singleQuestionButton->setChecked(true);
        correctAnswers.append(static_cast<SingleQuestion*>(question.get())->getGoodAnswerIndex());
        this->answerList->setAnswers(answers, correctAnswers, false);
    }
    onChangeQuestionType();
    this->image = pixmap;
    if(!pixmap.isNull())
        this->ui->imageGroupBox->setTitle(tr("Wybrano obraz"));
    else this->ui->imageGroupBox->setTitle(tr("Nie wybrano obrazu"));

    if(!maxTime.isNull()){
        this->ui->maxTimeEdit->setTime(maxTime);
        this->ui->timeLimitButton->setChecked(true);
    }
    else this->ui->timeLimitButton->setChecked(false);
    this->ui->questionTextEdit->setPlainText(question->getDescription());
    this->ui->pointsSpinBox->setValue(points);
    this->question = question;
    this->imageChanged = false;
}

void CreateQuestionWidget::reset()
{
    this->ui->singleQuestionButton->setChecked(true);
    this->ui->maxTimeEdit->setTime(QTime(0, 0, 10));
    this->ui->timeLimitButton->setChecked(false);
    this->ui->questionTextEdit->clear();
    this->image = QPixmap();
    this->answerList->removeAllAnswerWidgets();
    this->ui->imageGroupBox->setTitle(tr("Nie wybrano obrazu"));
    onChangeQuestionType();
    this->question = nullptr;
    this->imageChanged = false;
}

bool CreateQuestionWidget::isEmpty() const
{
    if(!ui->questionTextEdit->toPlainText().isEmpty()) return false;
    if(this->answerList->getValidAnswersCount() > 0) return false;
    if(this->isTimeSpecified) return false;
    if(!this->imgDialog->getPixmap().isNull()) return false;
    if(this->ui->pointsSpinBox->value() != 1) return false;

    return true;
}

void CreateQuestionWidget::onTimeSpecifiedChanged(bool value)
{
    this->ui->maxTimeEdit->setEnabled(value);
}

void CreateQuestionWidget::onConfirmClicked()
{

    if(this->ui->questionTextEdit->toPlainText().isEmpty()){
        QMessageBox::warning(this, tr("Błędne pytanie"), tr("Podaj treść pytania!"));
        return;
    }
    else if(this->answerList->getAnswers().size() < 2){
        QMessageBox::warning(this, tr("Błędne pytanie"), tr("Stwórz przynajmniej 2 odpowiedzi!"));
        return;
    }
    else if(!this->answerList->getCorrectAnswersCount() && this->ui->singleQuestionButton->isChecked()){
        QMessageBox::warning(this, tr("Błędne pytanie"), tr("Pytanie pojedyńcze musi mieć 1 poprawną odpowiedź!"));
        return;
    }
    emit questionConfirmed(this->getQuestion());

}

void CreateQuestionWidget::onChangeQuestionType()
{
    this->answerList->setAcceptingMultiCorrectAnswer(this->ui->multiQuestionButton->isChecked());
}

void CreateQuestionWidget::openImageDialog()
{
    imgDialog->setPixmap(this->image);
    int value = this->imgDialog->exec();
    if(value==QDialog::Accepted){
        image = imgDialog->getPixmap();
        if(image.isNull()){
            this->ui->imageGroupBox->setTitle(tr("Nie wybrano obrazu"));
        }
        else{
            this->ui->imageGroupBox->setTitle(tr("Wybrano obraz"));
        }
    }
}

void CreateQuestionWidget::onDenyClicked()
{
    if((!this->question.isNull() && (imageChanged || (*this->question.get())!=(*this->getQuestion().get())))||
            (this->question.isNull() && !this->isEmpty())){
        int button = QMessageBox::warning(this, tr("Niezapisane zmiany"), tr("Czy na pewno chcesz wyjść bez zapisania pytania?"),
                                          QMessageBox::Yes | QMessageBox::Cancel);
        if(button==QMessageBox::StandardButton::Yes){
            emit this->questionDenied();
        }
    }
    else{
        emit this->questionDenied();
    }
}

void CreateQuestionWidget::checkDescriptionLenght()
{
    QString content = this->ui->questionTextEdit->toPlainText();
    if(content.size() > MAX_DESCRIPTION_LENGTH){
        this->ui->questionTextEdit->textCursor().deletePreviousChar();
        return;
    }
}

void CreateQuestionWidget::onEditingAnswerChanged(EditAnswerWidget *widget)
{
    if(widget == nullptr) this->ui->confirmButton->setEnabled(true);
    else this->ui->confirmButton->setEnabled(false);
}
