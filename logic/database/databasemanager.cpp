#include "databasemanager.h"
#include "quizdatabase.h"
#include <QFileInfo>
#include <QtConcurrentMap>
#include <QtConcurrentRun>

DatabaseManager::DatabaseManager(const QString &directory)
    :dir(directory)
{
}

DatabaseManager::DatabaseManager(const QDir &directory)
    :dir(directory)
{
}

DatabaseManager::DatabaseManager(QDir &&directory)
    :dir(directory)
{
}
constexpr char DatabaseManager::invalidCharacters[];

bool DatabaseManager::saveQuiz(singleQuizPtr quiz, bool override)
{
    createDirIfNotExists();
    QString quizTitle = this->toDirName(quiz->getTitle());

    if(dir.exists(quizTitle)){
        if(override){
            QDir quizDir = dir;
            quizDir.cd(quizTitle);
            if(!quizDir.removeRecursively()) return false;
        }
        else return false;

    }
    dir.mkdir(quizTitle);
    QDir quizDir(dir.path() + '/' + quizTitle);
    return QuizDatabase::createQuizDatabase(quizDir.path(), quizTitle, quiz);
}

QFuture<bool> DatabaseManager::saveQuizAsync(singleQuizPtr quiz, bool override)
{
    QString title = QString(quiz->getTitle());

    auto worker = [override, quiz](const QString& path,const QString& title)->bool{
        QDir dir(path);
        dir.mkpath(dir.absolutePath());
        QString quizTitle = DatabaseManager::toDirName(title);

        if(dir.exists(quizTitle)){
            if(override){
                QDir quizDir = dir;
                quizDir.cd(quizTitle);
                if(!quizDir.removeRecursively()) return false;
            }
            else return false;

        }
        dir.mkdir(quizTitle);
        QDir quizDir(dir.path() + '/' + quizTitle);
        return QuizDatabase::createQuizDatabase(quizDir.path(), quizTitle, quiz);
    };

    return QtConcurrent::run(worker, dir.path(), title);
}

bool DatabaseManager::removeQuiz(const QString &title)
{
    QString quizTitle = this->toDirName(title);
    if(dir.exists(quizTitle)){
        QDir quizDir = dir;
        quizDir.cd(quizTitle);
        return quizDir.removeRecursively();
    }
    return true;
}

QList<singleQuizPtr> DatabaseManager::readAllQuizes()
{
    QList<singleQuizPtr> quizesList;

    //reading quizes folders
    QFileInfoList infoList = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for(const QFileInfo& info: infoList){
        QDir quizDir(info.filePath());
        QFileInfoList quizInfoList = quizDir.entryInfoList(QDir::Filter::Files);

        //searching database file
        for(const QFileInfo& quizInfo: quizInfoList){
            if(quizInfo.fileName() == quizDir.dirName() + ".db"){
                singleQuizPtr quiz = QuizDatabase::readQuizFromDatabase(quizInfo.filePath());
                if(!quiz.isNull())
                    quizesList.append(quiz);
            }
        }
    }
    return quizesList;
}

QList<singleQuizPtr> DatabaseManager::readAllQuizesAsync()
{
    QList<QuizPtrWithPath> quizesList;

    //reading quizes folders
    QFileInfoList infoList = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for(const QFileInfo& info: infoList){
        QDir quizDir(info.filePath());
        QFileInfoList quizInfoList = quizDir.entryInfoList(QDir::Filter::Files);

        //searching database file
        for(const QFileInfo& quizInfo: quizInfoList){
            if(quizInfo.fileName() == quizDir.dirName() + ".db"){
                quizesList.append(quizInfo.filePath());
            }
        }
    }
    return QtConcurrent::blockingMapped(quizesList, &QuizDatabase::readQuizFromDatabase);

}

bool DatabaseManager::canCreateThisQuiz(const QString &title)
{
    return !this->dir.exists(this->toDirName(title));
}

QStringList DatabaseManager::getActualFoldersNames() const
{
    QFileInfoList infoList = this->dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList list;
    for(const QFileInfo info :qAsConst(infoList)){
        list.append(info.fileName());
    }
    return list;
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

bool DatabaseManager::isQuizesDirectory(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()) return false;
    QFileInfoList entries = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    if(entries.isEmpty()) return false;

    QDir subDir;
    for(const auto& entry: qAsConst(entries)){
        subDir.setPath(entry.filePath());
        if(containsQuizDatabase(subDir)){
            return true;
        }
    }
    return false;
}

bool DatabaseManager::containsQuizDatabase(const QDir &dir)
{
    QFileInfoList entries = dir.entryInfoList(QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);
    for(const auto& entry: qAsConst(entries)){
        if(entry.fileName() == dir.dirName() + ".db"){
            return true;
        }
    }
    return false;
}

singleQuizPtr DatabaseManager::readQuizFromDir(const QDir &dir)
{
    return QuizDatabase::readQuizFromDatabase(dir.path() + "/" + dir.dirName() + ".db");
}

bool DatabaseManager::changeDirectory(const QString &path, bool removeOld)
{

    if(moveDirDirectly(this->dir.path(), path, removeOld)){
        this->dir.setPath(path);
        return true;
    }
    else{
        return false;
    }
}

bool DatabaseManager::changeDirectory(const QDir &directory, bool removeOld)
{
    if(moveDirDirectly(this->dir.path(), directory.path(), removeOld)){
        this->dir = directory;
        return true;
    }
    else{
        return false;
    }
}

bool DatabaseManager::changeDirectory(QDir &&directory, bool removeOld)
{
    if(moveDirDirectly(this->dir.path(), directory.path(), removeOld)){
        this->dir= directory;
        return true;
    }
    else{
        return false;
    }
}

void DatabaseManager::createDirIfNotExists()
{
    dir.mkpath(dir.absolutePath());
}

QString DatabaseManager::toDirName(const QString &title)
{
    QString quizTitle = title;
    for(int i=0 ;i<SIZE-1; i++){
        quizTitle.replace(DatabaseManager::invalidCharacters[i], replacementChar);
    }
    return quizTitle;
}
