#include "quizdatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QUuid>
#include <QMutex>
#include <QtConcurrent>
#include "./logic/multiquestion.h"
#include "./logic/singlequestion.h"
#include "./logic/database/databasemanager.h"

#define END {db.close(); return returnValue;}

QStringList QuizDatabase::initScripts;
QuizDatabase::QuizDatabase()
{
}

void QuizDatabase::readInitScripts()
{
    QFile file(":/database/sql/initDatabase.sql");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"Cannot read init file!";
        return;
    }

    QTextStream stream(&file);
    QString initString = stream.readAll();
    initScripts = initString.split(';');
    initScripts.removeAll("\n");
    file.close();
}

bool QuizDatabase::saveOneQuiz(QSqlQuery &query, singleQuizPtr quiz, const QDir& dir)
{
    Q_ASSERT(query.prepare("INSERT INTO quiz(quizTitle, maxTime, imgPath) VALUES(?,?,?)"));
    query.addBindValue(quiz->getTitle());
    query.addBindValue(quiz->getMaxTime());

    QString relativePath;
    QString imgSavePath;
    //saving quiz image
    QPixmap img = quiz->getTitleImage();

    auto imgWorker = [](const QPixmap& pixmap, const QString& fileName)->bool{
        return pixmap.save(fileName);
    };
    QList<QFuture<bool>> imgFutureList;

    if(!img.isNull()){
        dir.mkdir("img");
        relativePath = "img/" + QUuid::createUuid().toString() + ".png";
        imgSavePath = dir.path() + '/' + relativePath;
        imgFutureList.append(QtConcurrent::run(imgWorker, img, imgSavePath));
        query.addBindValue(relativePath);
    }
    else query.addBindValue(QVariant());

    bool value = query.exec();
    Q_ASSERT_X(value,"QuizDatabase::saveOneQuiz()", query.lastError().text().toStdString().c_str());
    if(!value) return false;;

    int quizId = query.lastInsertId().toInt();

    //saving questions
    for(int i=0; i<quiz->getQuestions().size(); i++){
        const auto& q = quiz->getQuestions().at(i);
        query.prepare("INSERT INTO questions(quizId, questionDescription, maxTime, imgPath, isMulti, points) VALUES(?,?,?,?,?,?)");
        query.addBindValue(quizId);
        query.addBindValue(q->getDescription());
        query.addBindValue(q->getTime());
        img = q->getImage();

        //saving image
        if(!img.isNull()){
            dir.mkdir("img");
            relativePath = "img/" + QUuid::createUuid().toString() + ".png";
            imgSavePath = dir.path() + '/' + relativePath;
            imgFutureList.append(QtConcurrent::run(imgWorker, img, imgSavePath));
            query.addBindValue(relativePath);
        }
        else query.addBindValue(QVariant());

        query.addBindValue(dynamic_cast<MultiQuestion*>(q.get())!=nullptr);
        query.addBindValue(q->getPoints());
        value = query.exec();
        Q_ASSERT(value);
        if(!value) return false;

        //saving answers
        int questionId = query.lastInsertId().toInt();

        for(int j=0; j<q->getAnswers().size(); j++){
            query.prepare("INSERT INTO answers(questionId, answerNumber, answerDescription, isGood) VALUES(?, ?, ?, ?)");
            query.addBindValue(questionId);
            query.addBindValue(j);
            query.addBindValue(q->getAnswers().at(j));
            query.addBindValue(q->isGoodAnswer(j));
            value = query.exec();
            Q_ASSERT(value);
            if(!value) return false;
        }

    }

    for(QFuture<bool>& future: imgFutureList){
        if(!future.result()) return false;
    }
    return true;
}

QPixmap QuizDatabase::loadPixmap(const QString &pathFromDatabase, const QString &pathToDatabase)
{
    QString imgPath = getImgPath(pathFromDatabase, pathToDatabase);
    QPixmap pixmap(imgPath, "PNG");
    return pixmap;
}

QString QuizDatabase::getImgPath(const QString &pathFromDatabase, const QString &pathToDatabase)
{
    QString dbDir = pathToDatabase.left(pathToDatabase.lastIndexOf('/'));
    QDir dir(dbDir);
    int lastIndex = pathFromDatabase.lastIndexOf('/');
    if(lastIndex < 0) lastIndex = 0;
    QString imgDir = pathFromDatabase.left(lastIndex);
    dir.cd(imgDir);
    return dir.path() + '/' + pathFromDatabase.right(pathFromDatabase.size() - 1 - pathFromDatabase.lastIndexOf('/'));
}

bool QuizDatabase::createQuizesDatabase(const QString &directory, const QString &dbName)
{
    //reading init script
    if(initScripts.isEmpty()) readInitScripts();
    QDir dir(directory);
    dir.mkpath(dir.absolutePath());

    static int connectionNumber = 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("createQuizesDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(directory + '/' + dbName);

    bool value = db.open();
    if(!value) return false;

    QSqlQuery query(db);
    bool returnValue = false;

    //initialize database with init script
    for(const auto& string: qAsConst(initScripts)){
        value = query.exec(string);
        Q_ASSERT(value);
        if(!value) END;
    }

    returnValue = true;
    END;

}

bool QuizDatabase::saveQuizToDatabase(const QString &dbPath, singleQuizPtr quiz)
{
    QFileInfo info(dbPath);
    if(!info.exists() && info.isFile()){
        Q_ASSERT(false);
        qDebug() <<"dbDir is not exist dbPath";
        return false;

    }
    static int connectionNumber = 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("saveQuizToDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dbPath);
    if(!db.open()) return false;

    QSqlQuery query(db);
    bool returnValue = saveOneQuiz(query, quiz, QFileInfo(dbPath).path());
    END;
}

QList<singleQuizPtr> QuizDatabase::saveQuizesToDatabase(const QString &dbPath,const QList<singleQuizPtr>& quizlist)
{
    QFileInfo info(dbPath);
    QList<singleQuizPtr> returnValue;
    if(!info.exists() && info.isFile()){
        Q_ASSERT(false);
        qDebug() <<"dbDir is not exist dbPath";
        return returnValue;

    }
    static int connectionNumber = 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("saveQuizesToDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dbPath);
    if(!db.open()) return returnValue;

    QSqlQuery query(db);
    QString dbFolder = QFileInfo(dbPath).path();
    for(const singleQuizPtr& ptr: qAsConst(quizlist)){
        if(QuizDatabase::saveOneQuiz(query, ptr, dbFolder)){
            returnValue.append(ptr);
        }
    }
    END;
}

bool QuizDatabase::removeQuizFromDatabase(const QString &dbPath, const QString &quizTitle)
{
    QFileInfo info(dbPath);
    if(!info.exists() && info.isFile()){
        Q_ASSERT(false);
        qDebug() <<"dbDir is not exist dbPath";
        return false;

    }
    static int connectionNumber = 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("removeQuizFromDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dbPath);
    if(!db.open()) return false;

    bool returnValue = false;
    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = ON");
    query.prepare("SELECT quizId, imgPath FROM quiz WHERE quizTitle='" + quizTitle + '\'');
    query.exec();


    if(query.next()){
        int quizId = query.value("quizId").toInt();
        QString imgPath = getImgPath(query.value("imgPath").toString(), dbPath);
        if(!imgPath.isEmpty()){
            QFile::remove(imgPath);
        }
        query.prepare("SELECT imgPath FROM questions WHERE quizId = " + QString::number(quizId));
        query.exec();
        while(query.next()){
            QString imgPath =getImgPath(query.value(0).toString(), dbPath);
            if(!imgPath.isEmpty()){
                QtConcurrent::run(&QFile::remove, imgPath);
            }
        }
    }

    query.prepare("DELETE FROM quiz WHERE quizTitle='" + quizTitle + '\'');

    if(query.exec()) returnValue = true;
    END;
}

QList<singleQuizPtr> QuizDatabase::readQuizesFromDatabase(const QString &dbPath)
{
    QList<singleQuizPtr> returnValue;
    static int connectionNumber = 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("readQuizesFromDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dbPath);
    if(!db.open()) return returnValue;

    QSqlQuery query1(db);
    //setting quiz
    if(!query1.exec("SELECT * FROM quiz")) END;

    QMap<singleQuizPtr, QFuture<QPixmap>> quizesImgages;
    QMap<basicQuestionPtr, QFuture<QPixmap>> questionImages;

    while(query1.next()){
        singleQuizPtr quiz(new SingleQuiz);
        quiz->setTitle(query1.value("quizTitle").toString());
        quiz->setMaxTime(query1.value("maxTime").toTime());

        QString imgPath = query1.value("imgPath").toString();
        if(!imgPath.isEmpty()){
            quizesImgages.insert(quiz, QtConcurrent::run(&QuizDatabase::loadPixmap,imgPath, dbPath));
            //  if(!pixmap.isNull()) quiz->setTitleImage(pixmap);
        }
        int quizId = query1.value("quizId").toInt();

        QSqlQuery query2(db);
        //setting questions
        if(!query2.exec("SELECT * FROM questions")){
            Q_ASSERT(false);
            END
        };
        QList<basicQuestionPtr> questions;
        QList<int> questionsIds;

        while(query2.next()){
            if(query2.value("quizId").toInt()!=quizId) continue;
            questionsIds.append(query2.value("questionId").toInt());

            basicQuestionPtr currentQuestion;
            if(query2.value("isMulti").toBool()){
                currentQuestion.reset(new MultiQuestion);
            }
            else{
                currentQuestion.reset(new SingleQuestion);
            }
            currentQuestion->setTime(query2.value("maxTime").toTime());
            currentQuestion->setDescription(query2.value("questionDescription").toString());
            currentQuestion->setPoints(query2.value("points").toInt());
            QString imgPath = query2.value("imgPath").toString();
            if(!imgPath.isEmpty()){
                questionImages.insert(currentQuestion, QtConcurrent::run(&QuizDatabase::loadPixmap,imgPath, dbPath));
            }
            questions.append(currentQuestion);
        }

        //setting answers
        for(int i=0; i<questionsIds.size(); i++){
            QSqlQuery query3(db);
            if(!query3.exec(QString("SELECT * FROM answers WHERE questionId=%1").arg(questionsIds.at(i)))){
                Q_ASSERT(false);
                END;
            }
            QStringList answers;
            QList<int> goodAnswersIndexes;
            int j = 0;
            while(query3.next()){
                answers<<query3.value("answerDescription").toString();
                if(query3.value("isGood").toBool()){
                    goodAnswersIndexes.append(j);
                }
                j++;
            }
            if(dynamic_cast<SingleQuestion*>(questions.at(i).get())!=nullptr){
                static_cast<SingleQuestion*>(questions.at(i).get())->setAnswers(std::move(answers), goodAnswersIndexes.at(0));
            }
            else{
                static_cast<MultiQuestion*>(questions.at(i).get())->setAnswers(std::move(answers), std::move(goodAnswersIndexes));
            }

        }

        quiz->setQuestions(questions);
        returnValue.append(quiz);
    }

    //saving images loaded in another threads
    for(auto i=quizesImgages.begin(); i!=quizesImgages.end(); i++){
        i.key()->setTitleImage(i.value().result());
    }

    for(auto i=questionImages.begin(); i!=questionImages.end(); i++){
        i.key()->setImage(i.value().result());
    }
    END;
}
