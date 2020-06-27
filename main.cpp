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

void changeNumberTo0(int& number){
    number = 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     MainWindow window;
     window.show();
    return a.exec();
}
