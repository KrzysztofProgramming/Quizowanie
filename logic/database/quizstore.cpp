#include "quizstore.h"
#include "./gui/mainwindow.h"
#include <QFutureWatcher>
#include <QFuture>

QuizStore::QuizStore(MainWindow *parent, const QString& dbPath) : QObject(parent)
  , mainWindow(parent)
  , dbManager(new DatabaseManager(dbPath))
{
    this->loadAllQuizes();
}

QuizStore::~QuizStore()
{
    delete dbManager;
}

bool QuizStore::removeQuiz(const singleQuizPtr &quiz)
{
    bool value = dbManager->removeQuiz(quiz->getTitle());
    if(value){
        this->quizes.removeAll(quiz);
        emit this->quizRemoved(quiz);
        return true;
    }
    return false;
}

void QuizStore::addQuizAsync(const singleQuizPtr &quiz)
{
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();
    watcher->setFuture(dbManager->saveQuizAsync(quiz));

    connect(watcher, &QFutureWatcher<bool>::finished, this, [=](){
        if(watcher->result()){
            if(watcher->result()){
                this->quizes.append(quiz);
                emit this->quizAdded(quiz);
            }
            else{
                emit this->addingFailed(quiz);
            }
        }
        watcher->deleteLater();
    });
}

void QuizStore::addQuizesAsync(const QList<singleQuizPtr> &quizes)
{
    auto *watcher = new QFutureWatcher<QList<singleQuizPtr>>;
    watcher->setFuture(this->dbManager->saveQuizes(quizes));

    connect(watcher, &QFutureWatcher<QList<singleQuizPtr>>::finished, this, [=](){
        QList<singleQuizPtr> watcherResult = watcher->result();
        this->quizes.append(watcherResult);
        QList<singleQuizPtr> notSavedQuizes = quizes;
        for(const singleQuizPtr& ptr: qAsConst(watcherResult)){
            emit this->quizAdded(ptr);
            notSavedQuizes.removeOne(ptr);
        }
        for(const singleQuizPtr& ptr: qAsConst(notSavedQuizes)){
            emit this->addingFailed(ptr);
        }
        watcher->deleteLater();
    });

}

void QuizStore::loadAllQuizes()
{
    this->quizes = this->dbManager->readAllQuizes();
}

bool QuizStore::changeDir(const QString &newDir, bool removeOld)
{
    return this->dbManager->changeDirectory(newDir, removeOld);
}

QString QuizStore::getDatabasePath() const
{
    return this->dbManager->getDatabasePath();
}

QString QuizStore::getDatabaseName() const
{
    return QFileInfo(this->dbManager->getDatabasePath()).fileName();
}

const QDir& QuizStore::getDir() const
{
    return this->dbManager->getDir();
}

QStringList QuizStore::getQuizesNames() const
{
    QStringList list;
    for(const singleQuizPtr& ptr: this->quizes){
        list.append(ptr->getTitle());
    }
    return list;
}
