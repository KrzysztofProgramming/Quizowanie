#ifndef BASICQUESTION_H
#define BASICQUESTION_H
#include <QPixmap>
#include <QTime>
#include <QStringList>
class SingleQuiz;
class BasicQuestion
{
public:
    BasicQuestion(const SingleQuiz* parent = nullptr);
    BasicQuestion(const QString& description, QStringList&& answers, const SingleQuiz* parent = nullptr);
    BasicQuestion(const QString& description, const QStringList& answers, const SingleQuiz* parent = nullptr);
    /**
     * @brief getAnswers
     * @return all available answers
     */
    inline const QStringList& getAnswers() const {return answers;}
    inline QStringList& getAnswers() {return answers;}
    /**
     * @brief getDescription
     * @return question description for example: "What's 2+2?";
     */
    inline const QString& getDescription() const {return description;}
    /**
     * @brief getImage
     * @return image which describe the queston, it might be empty image
     */
    inline const QPixmap& getImage() const {return descriptionImage;}
    /**
     * @brief getTime
     * @return time for giving answer or NULL time if it wasn't specified
     */
    inline const QTime& getTime() const {return time;}
    /**
     * @brief getPoints
     * @return points count which user gain when correctly answer this question
     */

    bool isMulti() const;
    inline int getPoints() const {return points;}
    inline bool isTimeSpecified() const {return !time.isNull();}
    inline bool hasImage() const {return !this->descriptionImage.isNull();}
    inline const SingleQuiz* getQuiz() const {return parent;}


    inline void setDescription(const QString& description){this->description = description;}
    inline void setImage(const QPixmap& image) {this->descriptionImage = image;}
    inline void setImage(QPixmap&& image){this->descriptionImage = image;}
    inline void setTime(const QTime& time){this->time = time;}
    inline void setPoints(int points){this->points = points;}
    virtual bool isGoodAnswer(int index) const = 0;
    virtual bool isValid() const;

    virtual ~BasicQuestion() {}
    /**
     * @brief operator == if two question have images, theire are not comparing, bacause of long comparison's time
     * @param other
     * @return
     */
    virtual bool operator==(const BasicQuestion& other);
    virtual bool operator!=(const BasicQuestion& other);
    friend SingleQuiz;

    virtual operator QString() const;

protected:
    QStringList answers;
    void setParent(const SingleQuiz* parent) {this->parent = parent;}

private:
    QString description = "";
    QPixmap descriptionImage;
    QTime time;
    int points = 1;
    const SingleQuiz *parent = nullptr;


};

using basicQuestionPtr = QSharedPointer<BasicQuestion>;


#endif // BASICQUESTION_H
