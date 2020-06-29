#ifndef QUIZSTORE_H
#define QUIZSTORE_H

#include <QObject>
#include "./logic/singlequiz.h"
#include "./logic/database/databasemanager.h"

QT_BEGIN_NAMESPACE
class MainWindow;

template <typename T>
class QFutureWatcher;
QT_END_NAMESPACE

class QuizStore : public QObject
{
    Q_OBJECT
public:
    explicit QuizStore(MainWindow *parent, const QString& dbDir);
    ~QuizStore();

    bool removeQuiz(const singleQuizPtr& quiz);
    bool editQuiz(const singleQuizPtr& oldQuiz, const singleQuizPtr& newQuiz);

    void addQuizAsync(const singleQuizPtr& quiz);
    void loadAllQuizes();
    bool changeDir(const QString& newDir, bool removeOld);
    QStringList getActualFoldersNames() const;
    const QDir& getDir() const;

    const QList<singleQuizPtr>& getAllQuizes() const {return quizes;}

signals:
    void quizAdded(singleQuizPtr quiz);
    void quizRemoved(singleQuizPtr quiz);
    void addingFailed(singleQuizPtr quiz);

private slots:
    void onWatcherFinished();
private:

    QList<singleQuizPtr> quizes;
    MainWindow *mainWindow;
    DatabaseManager *dbManager;

    QFutureWatcher<bool> *watcher;
    QFuture<bool> future;

    singleQuizPtr currentQuiz;
};

#endif // QUIZSTORE_H
