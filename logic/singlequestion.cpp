#include "singlequestion.h"
#include "singlequiz.h"
SingleQuestion::SingleQuestion(const SingleQuiz* parent)
    :BasicQuestion(parent)
{

}

SingleQuestion::SingleQuestion(const QString &description, const QStringList &answers, int goodAnswerIndex, const SingleQuiz *parent)
    :BasicQuestion(description, answers, parent)
{
    setGoodAnswer(goodAnswerIndex);
}

SingleQuestion::SingleQuestion(const QString &description, QStringList &&answers, int goodAnswerIndex, const SingleQuiz *parent)
    :BasicQuestion(description, answers, parent)
{
    setGoodAnswer(goodAnswerIndex);
}

void SingleQuestion::setAnswers(const QStringList &answers, int goodAnswerIndex)
{
    this->answers = answers;
    setGoodAnswer(goodAnswerIndex);
}

void SingleQuestion::setAnswers(QStringList &&answers, int goodAnswerIndex)
{
    this->answers = answers;
    setGoodAnswer(goodAnswerIndex);

}

void SingleQuestion::setGoodAnswer(int goodAnswerIndex)
{
    if(goodAnswerIndex < this->answers.size()){
        this->goodAnswerIndex = goodAnswerIndex;
    }
    else goodAnswerIndex = -1;
}

bool SingleQuestion::isGoodAnswer(int index) const
{
    return index == this->goodAnswerIndex;
}

QString SingleQuestion::getGoodAnswer() const
{
    if(goodAnswerIndex < 0){
        return QString();
    }
    return this->getAnswers().at(goodAnswerIndex);
}


bool SingleQuestion::isValid() const
{
    return goodAnswerIndex >= 0 && goodAnswerIndex < this->answers.size() && BasicQuestion::isValid();
}


bool SingleQuestion::operator ==(const BasicQuestion &other)
{
    if(dynamic_cast<const SingleQuestion*>(&other)){
        return goodAnswerIndex == static_cast<const SingleQuestion*>(&other)->goodAnswerIndex &&
                BasicQuestion::operator==(other);
    }
    else{
        return false;
    }
}

SingleQuestion::operator QString() const
{
    QString string = "SingleQuestion:\n" + BasicQuestion::operator QString() + 'n';
    string+="good answer: " + QString::number(goodAnswerIndex);
    return string;
}
