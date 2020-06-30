#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QDir>
#include "logic/singlequiz.h"
#include <QFuture>

class DatabaseManager
{
public:
    DatabaseManager(const QString& dbPath);
    static constexpr int SIZE = 11;
    static constexpr char invalidCharacters[SIZE] = R"(/\?"<>:*| )";
    static constexpr char replacementChar = '-';
    /**
     * @brief saveQuiz
     * @param quiz
     * @param override
     * @return true if quiz was save
     */

    bool saveQuiz(singleQuizPtr quiz, bool override = false);
    QFuture<bool> saveQuizAsync(singleQuizPtr quiz, bool override = false);
    QFuture<QList<singleQuizPtr>> saveQuizes(const QList<singleQuizPtr>& quizes);

    const QDir& getDir() const {return databaseDir;}
    const QString& getDatabasePath() const {return dbPath;}
    /**
     * @brief removeQuiz
     * @param title
     * @return true is quiz doesn't exist after this operation
     */
    bool removeQuiz(const QString& title);


    QList<singleQuizPtr> readAllQuizes();
    /**
     * @brief getActualFoldersNames
     * @return names of all catalogs in quiz directory
     */

    static bool clearDir(const QDir& databaseDir);

    //classic copy or paste in new dir is creating child with oldDir name
    static bool moveDir(const QString& oldDirectory, const QString& newDirectory, bool removeOldDir);

public:
    inline static bool copyDir(const QString& oldDirectory, const QString& newDirectory){
        return moveDir(oldDirectory, newDirectory, false);
    }

    inline static bool cutDir(const QString& oldDirectory, const QString& newDirectory){
        return moveDir(oldDirectory, newDirectory, true);
    }
    static bool canMove(const QDir& oldDirectory, const QDir& newDirectory);

    static bool moveDirDirectly(const QString &oldDirectory, const QString &newDirectory, bool removeOldDir);

private:
    static bool moveDirDirectlyWithoutChecking(const QString &oldDirectory, const QDir &newDirectory, bool removeOldDir);
    static bool moveDirWithoutChecking(const QString& oldDirectory, const QString& newDirectory, bool removeOldDir);

public:
    static bool isChildDirectory(const QDir& parent, const QDir& child);

    inline bool clearDir() {return clearDir(this->databaseDir);}
    bool changeDirectory(const QString &path, bool removeOld);

    //new dir is clear and copy every file from oldDir
private:
    //directory where are quizes folders
    QString dbPath;
    QDir databaseDir;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class QuizPtrWithPath{
    singleQuizPtr ptr = nullptr;
    QString pathToDatabase = "";

public:
    QuizPtrWithPath() = default;
    QuizPtrWithPath(singleQuizPtr ptr):ptr(ptr){}
    QuizPtrWithPath(const QString& path): pathToDatabase(path) {}

    void setPtr(const singleQuizPtr& ptr) {this->ptr = ptr;}
    void setPath(const QString& string) {this->pathToDatabase = string;}

    inline operator singleQuizPtr() const {return ptr;}
    inline operator const QString&() const {return pathToDatabase;}

};

#endif // DATABASEMANAGER_H
