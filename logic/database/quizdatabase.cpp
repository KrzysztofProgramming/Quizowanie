#include "quizdatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QMutex>
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

QPixmap QuizDatabase::loadPixmap(const QString &pathFromDatabase, const QString &pathToDatabase)
{
    QString dbDir = pathToDatabase.left(pathToDatabase.lastIndexOf('/'));
    QDir dir(dbDir);
    int lastIndex = pathFromDatabase.lastIndexOf('/');
    if(lastIndex < 0) lastIndex = 0;
    QString imgDir = pathFromDatabase.left(lastIndex);
    dir.cd(imgDir);
    QString imgPath = dir.path() + '/' + pathFromDatabase.right(pathFromDatabase.size() - 1 - pathFromDatabase.lastIndexOf('/'));
    QPixmap pixmap(imgPath, "PNG");
    return pixmap;
}

bool QuizDatabase::createQuizDatabase(const QString &directory, const QString &dbName, singleQuizPtr quiz)
{
    static int connectionNumber = 0;
    bool returnValue = false;

    QDir dir(directory);
    dir.mkpath(dir.absolutePath());
    bool value;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("creatQuizDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dir.path() + '/' + dbName + ".db");
    QSqlQuery query(db);

    //reading scrips if this is first time
    if(initScripts.isEmpty()) readInitScripts();
    db.open();

    for(const auto& string: qAsConst(initScripts)){
        value = query.exec(string);
        Q_ASSERT(value);
        if(!value) END;
    }

    //saving quiz to database
    query.prepare("INSERT INTO quiz(quizTitle, maxTime, imgPath) VALUES(?,?,?)");
    query.addBindValue(quiz->getTitle());
    query.addBindValue(quiz->getMaxTime());

    //saving quiz image
    QPixmap img = quiz->getTitleImage();
    if(!img.isNull()){
        dir.mkdir("img");
        QString imgSavePath = dir.path() + '/' + "img/title.png";
        img.save(imgSavePath, "PNG");
        query.addBindValue("img/title.png");
    }
    else query.addBindValue(QVariant());

    value = query.exec();
    Q_ASSERT(value);
    if(!value) END;

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
            QString imgSavePath = dir.path() + '/' + QString("img/question%1.png").arg(i);
            img.save(imgSavePath, "PNG");
            query.addBindValue(QString("img/question%1.png").arg(i));
        }
        else query.addBindValue(QVariant());

        query.addBindValue(dynamic_cast<MultiQuestion*>(q.get())!=nullptr);
        query.addBindValue(q->getPoints());
        value = query.exec();
        Q_ASSERT(value);
        if(!value) END;

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
            if(!value) END;
        }

    }

    db.close();
    return true;
}

singleQuizPtr QuizDatabase::readQuizFromDatabase(const QString &dbPath)
{
    static int connectionNumber = 0;
    singleQuizPtr returnValue = nullptr;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("readQuizFromDatabase%1").arg(++connectionNumber));
    db.setDatabaseName(dbPath);
    if(!db.open()) return nullptr;

    QSqlQuery query(db);
    //setting quiz
    if(!query.exec("SELECT * FROM quiz LIMIT 1")) END;
    query.next();
    singleQuizPtr quiz(new SingleQuiz);
    quiz->setTitle(query.value("quizTitle").toString());
    quiz->setMaxTime(query.value("maxTime").toTime());

    QString imgPath = query.value("imgPath").toString();
    if(!imgPath.isEmpty()){
        QPixmap pixmap = loadPixmap(imgPath, dbPath);
        if(!pixmap.isNull()) quiz->setTitleImage(pixmap);
    }
    int quizId = query.value("quizId").toInt();

    //setting questions
    if(!query.exec("SELECT * FROM questions")) END;
    QList<basicQuestionPtr> questions;
    QList<int> questionsIds;

    while(query.next()){
        if(query.value("quizId").toInt()!=quizId) continue;
        questionsIds.append(query.value("questionId").toInt());

        basicQuestionPtr currentQuestion;
        if(query.value("isMulti").toBool()){
            currentQuestion.reset(new MultiQuestion);
        }
        else{
            currentQuestion.reset(new SingleQuestion);
        }
        currentQuestion->setTime(query.value("maxTime").toTime());
        currentQuestion->setDescription(query.value("questionDescription").toString());
        currentQuestion->setPoints(query.value("points").toInt());
        QString imgPath = query.value("imgPath").toString();
        if(!imgPath.isEmpty()){
            QPixmap pixmap = loadPixmap(imgPath, dbPath);
            if(!pixmap.isNull()) currentQuestion->setImage(pixmap);
        }
        questions.append(currentQuestion);
    }

//setting answers
    for(int i=0; i<questionsIds.size(); i++){
        if(!query.exec(QString("SELECT * FROM answers WHERE questionId=%1").arg(questionsIds.at(i)))) END;
        QStringList answers;
        QList<int> goodAnswersIndexes;
        int j = 0;
        while(query.next()){
            answers<<query.value("answerDescription").toString();
            if(query.value("isGood").toBool()){
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
    db.close();
    return quiz;
}
