#include "databasemanager.h"
#include "quizdatabase.h"
#include <QFileInfo>
#include <QtConcurrentMap>
#include <QtConcurrentRun>
#include <QtConcurrentFilter>

DatabaseManager::DatabaseManager(const QString &dbPath)
{
    QFileInfo info(dbPath);
    this->dbPath = dbPath;
    this->databaseDir.setPath(info.path());
    if(!info.exists() || !info.isFile()){
        QString path = this->databaseDir.path();
        Q_ASSERT(QuizDatabase::createQuizesDatabase(databaseDir.path(), info.fileName()));
    }
}

constexpr char DatabaseManager::invalidCharacters[];

bool DatabaseManager::saveQuiz(singleQuizPtr quiz, bool override)
{
    if(override) QuizDatabase::removeQuizFromDatabase(this->dbPath, quiz->getTitle());
    return QuizDatabase::saveQuizToDatabase(this->dbPath, quiz);
}

QFuture<bool> DatabaseManager::saveQuizAsync(singleQuizPtr quiz, bool override)
{   
    auto worker = [override, quiz](const QString& path)->bool{
        if(override) QuizDatabase::removeQuizFromDatabase(path, quiz->getTitle());
        return QuizDatabase::saveQuizToDatabase(path, quiz);
    };

    return QtConcurrent::run(worker, this->dbPath);
}

QFuture<QList<singleQuizPtr>> DatabaseManager::saveQuizes(const QList<singleQuizPtr> &quizes)
{
    return QtConcurrent::run(&QuizDatabase::saveQuizesToDatabase, this->dbPath, quizes);
}

bool DatabaseManager::removeQuiz(const QString &title)
{
    return QuizDatabase::removeQuizFromDatabase(this->dbPath, title);
}

QList<singleQuizPtr> DatabaseManager::readAllQuizes()
{
    return QuizDatabase::readQuizesFromDatabase(this->dbPath);
}


bool DatabaseManager::clearDir(const QDir& dir)
{
    QFileInfoList infoList = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    bool value = true;
    for(const QFileInfo& info: infoList){
        QDir dirToClear(info.filePath());
        value = dirToClear.removeRecursively() && value;
    }
    return value;
}
bool DatabaseManager::moveDir(const QString &oldDirectory, const QString &newDirectory, bool removeOldDir)
{
    QDir oldDir(oldDirectory);
    if(!oldDir.exists()) return false;

    QDir newDir(newDirectory);
    if(!newDir.exists()) return false;

    if(!canMove(oldDirectory, newDirectory))
        return false;

    if(isChildDirectory(oldDir, newDir)){
        return false;
    }
    if(newDir.exists(oldDir.dirName())) return false;

    return moveDirWithoutChecking(oldDirectory, newDirectory, removeOldDir);
    
}

bool DatabaseManager::moveDirWithoutChecking(const QString &oldDirectory, const QString &newDirectory, bool removeOldDir)
{
    QDir oldDir(oldDirectory);
    QDir newDir(newDirectory);

    newDir.mkdir(oldDir.dirName());
    newDir.cd(oldDir.dirName());

    QFileInfoList entries = oldDir.entryInfoList(QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for(const auto& entry: qAsConst(entries)){
        QString newPath = newDir.path() + '/' + entry.fileName();
        if(entry.isFile()){
            if(!QFile::copy(entry.filePath(), newPath))
                return false;
        }
        else if(entry.isDir()){
            if(!moveDirWithoutChecking(entry.filePath(), newDir.path(), false))
                return false;
        }
        else{
            ;
        }
    }

    if(removeOldDir){
        return oldDir.removeRecursively();
    }
    return true;
}

bool DatabaseManager::canMove(const QDir &oldDir, const QDir &newDir)
{
    if(!oldDir.exists()) return false;

    if(!newDir.exists()) return false;

    QString absPath = oldDir.absolutePath();
    QString simulatingPath = newDir.absolutePath() + '/' + absPath.right(absPath.size() - absPath.lastIndexOf('/'));

    QFileInfo info(simulatingPath);
    return !info.exists();
}

bool DatabaseManager::moveDirDirectly(const QString &oldDirectory, const QString &newDirectory, bool removeOldDir)
{
    QDir oldDir(oldDirectory);
    if(!oldDir.exists()) return false;

    QDir newDir(newDirectory);

    if(isChildDirectory(oldDir, newDir)){
        return false;
    }

    return moveDirDirectlyWithoutChecking(oldDirectory, newDir, removeOldDir);

}

bool DatabaseManager::moveDirDirectlyWithoutChecking(const QString &oldDirectory, const QDir &newDir, bool removeOldDir)
{
    QDir oldDir(oldDirectory);
    if(!newDir.mkpath(newDir.absolutePath())){
        return false;
    }
    QFileInfoList entries = oldDir.entryInfoList(QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for(const auto& entry: qAsConst(entries)){
        QString newPath = newDir.path() + '/' + entry.fileName();
        if(entry.isFile()){
            if(!QFile::copy(entry.filePath(), newPath)){
                qDebug() <<"false";
                return false;
            }
        }
        else if(entry.isDir()){
            if(!moveDirDirectlyWithoutChecking(entry.filePath(), newPath, false)){
                qDebug() <<"false";
                return false;
            }
        }
        else{
            ;
        }
    }

    if(removeOldDir){
        return oldDir.removeRecursively();
    }
    return true;

}

bool DatabaseManager::isChildDirectory(const QDir &parent, const QDir &child)
{
    if(parent.absolutePath() == child.absolutePath()) return true;
    QFileInfoList list = parent.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(const QFileInfo& info: qAsConst(list)){
        if(isChildDirectory(QDir(info.absoluteFilePath()), child)){
            return true;
        }
    }
    return false;
}

bool DatabaseManager::changeDirectory(const QString &path, bool removeOld)
{
    QString newDbPath =  path + '/' + QFileInfo(dbPath).fileName();
    bool value = QFile::copy(this->dbPath, newDbPath);
    if(value){
        if(moveDirDirectly(this->databaseDir.path() + "/img", path + "/img", removeOld)){
            this->databaseDir.setPath(path);
            this->dbPath = newDbPath;
            return true;
        }
    }
    return false;
}
