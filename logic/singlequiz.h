#ifndef SINGLEQUIZ_H
#define SINGLEQUIZ_H
#include "basicquestion.h"

class SingleQuiz
{
public:
    SingleQuiz();
    SingleQuiz(const QString& title, const QList<basicQuestionPtr>& questions);
    SingleQuiz(const QString& title, QList<basicQuestionPtr>&& questions);
    /**
     * @brief getMaxTime
     * @return time to solve the quize or NULL time if it wasn't specified, if was single question time won't be analyze
     */
    inline const QTime& getMaxTime() const {return maxTime;}
    /**
     * @brief getTitleImage
     * @return title image of this quize, it might be empty
     */
    inline const QPixmap& getTitleImage() const {return titleImage;}
    /**
     * @brief getTitle
     * @return title of this quiz
     */
    inline const QString& getTitle() const {return title;}
    /**
     * @brief getQuestions const
     * @return const QList of quiz questions
     */
    inline const QList<basicQuestionPtr>& getQuestions() const {return questions;}
    /**
     * @brief getQuestion
     * @return  QList of quiz questions
     */
    inline void addQuestion(basicQuestionPtr question) {this->questions.append(question), question->setParent(this);}
    //inline QList<basicQuestionPtr>& getQuestions() {return questions; setParentForQuestions();}

    inline bool isTimeSpecified() const {return !maxTime.isNull();}
    inline bool hasImage() const {return !titleImage.isNull();}
    int getMaxPoints();

    void setMaxTime(const QTime& time){this->maxTime = time;}
    void setTitle(const QString& title){this->title = title;}
    void setQuestions(const QList<basicQuestionPtr>& questions){this->questions = questions; setParentForQuestions();}
    void setQuestions(QList<basicQuestionPtr>&& questions){this->questions = questions; setParentForQuestions();}
    void setTitleImage(const QPixmap& image){this->titleImage = image;}
    void setTitleImage(QPixmap&& image){this->titleImage = image;}

    inline bool isValid() const {return !title.isEmpty() && questions.size() > 0;}
    bool operator==(const SingleQuiz& other) const;
    bool operator!=(const SingleQuiz& other) const;
    operator QString() const;

private:
    QString title="";
    QPixmap titleImage;
    QList<basicQuestionPtr> questions;
    QTime maxTime;

    void setParentForQuestions();
};
using singleQuizPtr = QSharedPointer<SingleQuiz>;
#endif // SINGLEQUIZ_H
