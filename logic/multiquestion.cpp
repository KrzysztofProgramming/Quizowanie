#include "multiquestion.h"

MultiQuestion::MultiQuestion(const SingleQuiz *parent)
    :BasicQuestion(parent)
{
}

MultiQuestion::MultiQuestion(const QString &description, const QStringList &answers, std::initializer_list<int> goodAnswerIndexes, const SingleQuiz *parent)
    :BasicQuestion(description, answers, parent)
{
    setGoodAnswerIndexes(goodAnswerIndexes);
}

MultiQuestion::MultiQuestion(const QString &description, QStringList &&answers, std::initializer_list<int> goodAnswerIndexes, const SingleQuiz *parent)
    :BasicQuestion(description, answers, parent)
{
    setGoodAnswerIndexes(goodAnswerIndexes);
}

MultiQuestion::MultiQuestion(const QString &description, const QStringList &answers, const QList<int> &goodAnswerIndexes, const SingleQuiz *parent)
    :BasicQuestion(description, answers, parent)
{
    setGoodAnswerIndexes(goodAnswerIndexes);
}

void MultiQuestion::setAnswers(QStringList &&answers, std::initializer_list<int> goodAnswerIndexes)
{
    this->answers = answers;
    setGoodAnswerIndexes(goodAnswerIndexes);
}

void MultiQuestion::setAnswers(const QStringList &answers, std::initializer_list<int> goodAnswerIndexes)
{
    this->answers = answers;
    setGoodAnswerIndexes(goodAnswerIndexes);
}

void MultiQuestion::setAnswers(QStringList &&answers, QList<int> &&goodAnswerIndexes)
{
    this->answers = answers;
    setGoodAnswerIndexes(goodAnswerIndexes);
}

void MultiQuestion::setAnswers(const QStringList &answers, const QList<int> &goodAnswerIndexes)
{
    this->answers = answers;
    setGoodAnswerIndexes(goodAnswerIndexes);
}

void MultiQuestion::setGoodAnswerIndexes(const std::initializer_list<int> &list)
{
    goodAnswerIndexes.clear();
    for(auto x: list){
        if(x<this->answers.size()){
            this->goodAnswerIndexes.append(x);
        }
    }
}

void MultiQuestion::setGoodAnswerIndexes(const QList<int> &list)
{
    goodAnswerIndexes.clear();
    for(auto x: list){
        if(x<this->answers.size()){
            this->goodAnswerIndexes.append(x);
        }
    }
}

bool MultiQuestion::isGoodAnswer(int index) const
{
    return this->goodAnswerIndexes.contains(index);
}


bool MultiQuestion::operator ==(const BasicQuestion &other)
{
    if(dynamic_cast<const MultiQuestion*>(&other)){
        return goodAnswerIndexes == static_cast<const MultiQuestion*>(&other)->goodAnswerIndexes &&
                BasicQuestion::operator==(other);
    }
    else{
        return false;
    }
}

MultiQuestion::operator QString() const
{
    QString string = "MultiQuestion:\n" + BasicQuestion::operator QString() + '\n';
    string + "good answers: (";
    for(const int& x: qAsConst(goodAnswerIndexes)){
        string += QString::number(x) + " ";
    }
    string +=')';
    return string;
}
