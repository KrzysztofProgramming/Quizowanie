#ifndef QUIZDATABASE_H
#define QUIZDATABASE_H

#include "./logic/singlequiz.h"
class QMutex;
class QuizPtrWithPath;
class QuizDatabase
{
public:
    QuizDatabase();
    static bool createQuizDatabase(const QString& directory, const QString& dbName, singleQuizPtr quiz);
    static singleQuizPtr readQuizFromDatabase(const QString& dbPath);

private:
    static QStringList initScripts;

    static void readInitScripts();
    static QPixmap loadPixmap(const QString& pathFromDatabase, const QString& pathToDatabase);
};

#endif // QUIZDATABASE_H
