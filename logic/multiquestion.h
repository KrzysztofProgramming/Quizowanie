#ifndef MULTIQUESTION_H
#define MULTIQUESTION_H
#include "basicquestion.h"

class MultiQuestion : public BasicQuestion
{
public:
    MultiQuestion(const SingleQuiz* parent = nullptr);
    MultiQuestion(const QString& description, const QStringList& answers, std::initializer_list<int> goodAnswerIndexes, const SingleQuiz* parent = nullptr);
    MultiQuestion(const QString& description, QStringList&& answers, std::initializer_list<int> goodAnswerIndexes, const SingleQuiz* parent = nullptr);
    MultiQuestion(const QString& description, const QStringList& answers, const QList<int>& goodAnswerIndexes, const SingleQuiz* parent = nullptr);

    void setAnswers(QStringList&& answers, std::initializer_list<int> goodAnswerIndexes);
    void setAnswers(const QStringList& answers, std::initializer_list<int> goodAnswerIndexes);
    void setAnswers(QStringList&& answers, QList<int>&& goodAnswerIndexes);
    void setAnswers(const QStringList& answers, const QList<int>& goodAnswerIndexes);

    inline int getAnswersCount() const {return goodAnswerIndexes.size();}
    inline const QList<int>& getGoodAnswersIndexes() const {return goodAnswerIndexes;}
private:
    QList<int> goodAnswerIndexes;

    void setGoodAnswerIndexes(const std::initializer_list<int>& list);
    void setGoodAnswerIndexes(const QList<int>& list);

    // BasicQuestion interface
public:
    virtual bool isGoodAnswer(int index) const override;
    virtual bool operator ==(const BasicQuestion &other) override;
    virtual operator QString() const override;
};

using multiQuestionPtr = QSharedPointer<MultiQuestion>;

#endif // MULTIQUESTION_H
