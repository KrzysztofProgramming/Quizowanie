#include "gui/mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QtConcurrentMap>

#include "logic/multiquestion.h"
#include "logic/singlequestion.h"
#include "gui/quiz_widgets/singlequizwidget.h"
#include "logic/database/quizdatabase.h"
#include "logic/database/databasemanager.h"
#include "gui/quiz_widgets/quizareadisplay.h"
#include "gui/quiz_widgets/quiziconwidget.h"
#include "gui/others/flowlayout.h"
#include "gui/answer_widgets/editanswerwidget.h"
#include "gui/question_widgets/createquestionwidget.h"
#include "gui/quiz_widgets/createquizwidget.h"
#include "gui/simple_widgets/quizcreatorwidget.h"
#include "./gui/simple_widgets/changepathdialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
   /* singleQuizPtr quiz(new SingleQuiz);
    QStringList list;

    singleQuestionPtr q1(new SingleQuestion);
    q1->setDescription("Ile to 2+2?");
    list<<"4"<<"5"<<"545"<<"asdsad";
    q1->setAnswers(std::move(list), 0);
    list.clear();

    multiQuestionPtr q2(new MultiQuestion);
    q2->setDescription("Ile lat ma adaś?");
    list<<"trzy"<<"3"<<"trzysta"<<"300";
    q2->setAnswers(std::move(list), {0,1});
    list.clear();

    singleQuestionPtr q3(new SingleQuestion);
    q3->setDescription("Czy Lalka jest fajna?");
    list<<"nie xd"<<"nie"<<"tak xd"<<"Zajebista";
    q3->setAnswers(std::move(list), 3);
    list.clear();

    QList<basicQuestionPtr> qList;
    qList.append(q1);
    qList.append(q2);
    qList.append(q3);
    quiz->setQuestions(qList);
    quiz->setTitle("testowy");*/
  /*  singleQuizPtr quiz1 = QuizDatabase::readQuizFromDatabase("C:/Users/Krzychu/Desktop/quizy/Czy znasz władców Polski-/Czy znasz władców Polski-.db");
    singleQuizPtr quiz2 = QuizDatabase::readQuizFromDatabase("C:/Users/Krzychu/Desktop/quizy/Matematyka/Matematyka.db");
    singleQuizPtr quiz3 = QuizDatabase::readQuizFromDatabase("C:/Users/Krzychu/Desktop/quizy/Programowanie/Programowanie.db");
    singleQuizPtr quiz4 = QuizDatabase::readQuizFromDatabase("C:/Users/Krzychu/Desktop/quizy/Zamki/Zamki.db");

    qDebug() <<*quiz1;
    qDebug() <<*quiz2;
    qDebug() <<*quiz3;
    qDebug() <<*quiz4;
    if(quiz1 == nullptr || quiz2 == nullptr || quiz3 == nullptr || quiz4 == nullptr) return -1;
    QuizDatabase::createQuizesDatabase("dbFolder", "quizDb.db");
    QuizDatabase::saveQuizToDatabase("dbFolder/quizDb.db", quiz1);
    QuizDatabase::saveQuizToDatabase("dbFolder/quizDb.db", quiz2);
    QuizDatabase::saveQuizToDatabase("dbFolder/quizDb.db", quiz3);
    QuizDatabase::saveQuizToDatabase("dbFolder/quizDb.db", quiz4);*/
  // qDebug()<< QuizDatabase::removeQuizFromDatabase("dbFolder/quizDb.db","testowy123");


    return a.exec();
}
