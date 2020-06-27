#include "singlequiz.h"
SingleQuiz::SingleQuiz()
{

}

SingleQuiz::SingleQuiz(const QString &title, const QList<basicQuestionPtr>& questions)
    :title(title), questions(questions)
{
    setParentForQuestions();
}
SingleQuiz::SingleQuiz(const QString &title, QList<basicQuestionPtr>&& questions)
    :title(title), questions(questions)
{
    setParentForQuestions();
}

int SingleQuiz::getMaxPoints()
{
    int points = 0;
    for(int i=0; i<questions.size(); i++){
        points += questions.at(i)->getPoints();
    }
    return points;
}

bool SingleQuiz::operator==(const SingleQuiz &other) const
{
    if(this == &other) return true;
    if(title != other.title) return false;
    if(maxTime !=  other.maxTime) return false;
    if(titleImage.isNull() != other.titleImage.isNull()) return false;
    if(questions.size() != other.questions.size()) return false;
    for(int i=0; i<questions.size(); i++){
        if(*(questions.at(i).get()) != *(other.questions.at(i).get())){
            return false;
        }
    }
    return true;
}

bool SingleQuiz::operator!=(const SingleQuiz &other) const
{
    return !(*this==other);
}

SingleQuiz::operator QString() const
{
    QString string = "Quiz: " + title + '\n';
    string += "time: " + maxTime.toString("hh:mm:ss") + '\n';
    string += "image: " + QString::number(titleImage.size().width()) + 'x' + QString::number(titleImage.size().height()) + '\n';
    for(const basicQuestionPtr& ptr: qAsConst(questions)){
        string += '\n' + *ptr;
    }
    return string;
}

void SingleQuiz::setParentForQuestions()
{
    for(int i=0; i<this->questions.size(); i++){
        this->questions.at(i)->setParent(this);
    }
}
