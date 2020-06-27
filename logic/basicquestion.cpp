#include "basicquestion.h"
#include "multiquestion.h"

BasicQuestion::BasicQuestion(const SingleQuiz* parent)
    :parent(parent)
{

}

BasicQuestion::BasicQuestion(const QString &description, QStringList &&answers, const SingleQuiz* parent)
    :answers(answers), description(description), parent(parent)
{

}

BasicQuestion::BasicQuestion(const QString &description, const QStringList &answers, const SingleQuiz* parent)
    :answers(answers), description(description),  parent(parent)
{

}

bool BasicQuestion::isMulti() const
{
    return dynamic_cast<const MultiQuestion*>(this) != nullptr;
}

bool BasicQuestion::isValid() const
{
    return this->answers.size() >= 2 && !description.isEmpty() && points > 0;
}

bool BasicQuestion::operator==(const BasicQuestion &other)
{
    if(this == &other) return true;
    if(answers != other.answers) return false;
    if(description != other.description) return false;
    if(time != other.time) return false;
    if(points != other.points) return false;
    if(descriptionImage.isNull() != other.descriptionImage.isNull()) return false;

    return true;
}

bool BasicQuestion::operator!=(const BasicQuestion &other)
{
    return !this->operator==(other);
}

BasicQuestion::operator QString() const
{
    QString string = description + '\n';
    string += "time: " + time.toString("hh:mm:ss") + '\n';
    string += "image: " +QString::number(descriptionImage.size().width()) + 'x' + QString::number(descriptionImage.size().height()) + '\n';
    string += "points: " + QString::number(this->points);
    for(const QString& answer: qAsConst(answers)){
        string += '\n' + answer;
    }
    return string;
}
