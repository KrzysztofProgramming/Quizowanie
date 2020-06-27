#ifndef QUESTION_H
#define QUESTION_H
#include "basicquestion.h"
class SingleQuiz;

class SingleQuestion : public BasicQuestion
{
public:
    SingleQuestion(const SingleQuiz* parent = nullptr);
    SingleQuestion(const QString& description, const QStringList& answers, int goodAnswerIndex, const SingleQuiz* parent = nullptr);
    SingleQuestion(const QString& description, QStringList&& answers, int goodAnswerIndex, const SingleQuiz* parent = nullptr);

    inline int getGoodAnswerIndex() const {return goodAnswerIndex;}
    inline bool hasAnswer() const {return goodAnswerIndex < 0;}

    void setAnswers(const QStringList& answers, int goodAnswerIndex);
    void setAnswers(QStringList&& answers, int goodAnswerIndex);

    QString getGoodAnswer() const;

private:
    int goodAnswerIndex = -1;

    void setGoodAnswer(int goodAnswer);


    // BasicQuestion interface
public:
    virtual bool isGoodAnswer(int index) const override;
    virtual bool isValid() const override;
    virtual bool operator ==(const BasicQuestion &other) override;\
    virtual operator QString() const override;
};

using singleQuestionPtr = QSharedPointer<SingleQuestion>;

#endif // QUESTION_H
