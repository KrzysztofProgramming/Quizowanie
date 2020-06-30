#ifndef QUIZDATABASE_H
#define QUIZDATABASE_H

#include "./logic/singlequiz.h"

QT_BEGIN_NAMESPACE
class QMutex;
class QuizPtrWithPath;
class QSqlQuery;
class QDir;
QT_END_NAMESPACE

class QuizDatabase
{
public:
    QuizDatabase();


    static bool createQuizesDatabase(const QString& directory, const QString& dbName);

    static bool saveQuizToDatabase(const QString& dbPath, singleQuizPtr quiz);
    /**
     * @brief saveQuizesToDatabase
     * @param dbPath
     * @param quizList
     * @return list of savedQuizes
     */
    static QList<singleQuizPtr> saveQuizesToDatabase(const QString& dbPath, const QList<singleQuizPtr>& quizList);
    static bool removeQuizFromDatabase(const QString& dbPath, const QString& quizTitle);

    static QList<singleQuizPtr> readQuizesFromDatabase(const QString& dbPath);

private:
    static QStringList initScripts;

    static void readInitScripts();
    static bool saveOneQuiz(QSqlQuery& openedQuery, singleQuizPtr quiz, const QDir& dbPath);

    static QPixmap loadPixmap(const QString& pathFromDatabase, const QString& pathToDatabase);
    static QString getImgPath(const QString &pathFromDatabase, const QString &pathToDatabase);
};

#endif // QUIZDATABASE_H
