#include "quizstore.h"
#include "./gui/mainwindow.h"
#include <QFutureWatcher>
#include <QFuture>
#define CHECK_WATCHER if(watcher->isRunning()) return false;

QuizStore::QuizStore(MainWindow *parent, const QString& dbDir) : QObject(parent)
  , mainWindow(parent)
  , dbManager(new DatabaseManager(dbDir))
  , watcher(new QFutureWatcher<bool>(this))
{
    this->loadAllQuizes();
    connect(watcher, &QFutureWatcher<bool>::finished, this, &QuizStore::onWatcherFinished);
}

QuizStore::~QuizStore()
{
    delete dbManager;
}

bool QuizStore::removeQuiz(const singleQuizPtr &quiz)
{
    CHECK_WATCHER;
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
    if(watcher->isRunning()){
        emit this->addingFailed(quiz);
        return;
    }
    this->currentQuiz = quiz;
    watcher->setFuture(dbManager->saveQuizAsync(quiz));
}

void QuizStore::loadAllQuizes()
{
    this->quizes = this->dbManager->readAllQuizesAsync();
}

bool QuizStore::changeDir(const QString &newDir, bool removeOld)
{
    return this->dbManager->changeDirectory(newDir, removeOld);
}

QStringList QuizStore::getActualFoldersNames() const
{
    return this->dbManager->getActualFoldersNames();
}

const QDir& QuizStore::getDir() const
{
    return this->dbManager->getDir();
}

void QuizStore::onWatcherFinished()
{
    if(watcher->result()){
        this->quizes.append(this->currentQuiz);
        emit this->quizAdded(this->currentQuiz);
    }
    else{
        emit this->addingFailed(this->currentQuiz);
    }
    this->currentQuiz = nullptr;
}
