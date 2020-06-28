QT += core gui
QT += sql
QT += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_ICONS = ikona.ico

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gui/answer_widgets/answerslistwidget.cpp \
    gui/answer_widgets/editanswerwidget.cpp \
    gui/others/flowlayout.cpp \
    gui/question_widgets/basicquestionwidget.cpp \
    gui/question_widgets/createquestionwidget.cpp \
    gui/question_widgets/questionlistelement.cpp \
    gui/question_widgets/questionlistwidget.cpp \
    gui/quiz_widgets/createquizwidget.cpp \
    gui/quiz_widgets/multiquizwidget.cpp \
    gui/quiz_widgets/quizareadisplay.cpp \
    gui/quiz_widgets/quiziconwidget.cpp \
    gui/quiz_widgets/quizrepresentationwidget.cpp \
    gui/quiz_widgets/quizsummarywidget.cpp \
    gui/quiz_widgets/singlequizwidget.cpp \
    gui/simple_widgets/aboutdialog.cpp \
    gui/simple_widgets/busylabel.cpp \
    gui/simple_widgets/changepathdialog.cpp \
    gui/simple_widgets/imagedialog.cpp \
    gui/simple_widgets/mainmenu.cpp \
    gui/simple_widgets/pixmaplabel.cpp \
    gui/simple_widgets/quizcreatorwidget.cpp \
    gui/simple_widgets/titlewidget.cpp \
    logic/basicquestion.cpp \
    logic/database/databasemanager.cpp \
    logic/database/quizdatabase.cpp \
    logic/database/quizstore.cpp \
    logic/multiquestion.cpp \
    logic/singlequestion.cpp \
    logic/singlequiz.cpp \
    main.cpp \
    gui/mainwindow.cpp

HEADERS += \
    gui/answer_widgets/answerslistwidget.h \
    gui/answer_widgets/editanswerwidget.h \
    gui/others/flowlayout.h \
    gui/question_widgets/basicquestionwidget.h \
    gui/question_widgets/createquestionwidget.h \
    gui/question_widgets/questionlistelement.h \
    gui/question_widgets/questionlistwidget.h \
    gui/quiz_widgets/createquizwidget.h \
    gui/quiz_widgets/multiquizwidget.h \
    gui/quiz_widgets/quizareadisplay.h \
    gui/quiz_widgets/quiziconwidget.h \
    gui/quiz_widgets/quizrepresentationwidget.h \
    gui/quiz_widgets/quizsummarywidget.h \
    gui/quiz_widgets/singlequizwidget.h \
    gui/simple_widgets/aboutdialog.h \
    gui/simple_widgets/busylabel.h \
    gui/simple_widgets/changepathdialog.h \
    gui/simple_widgets/imagedialog.h \
    gui/simple_widgets/mainmenu.h \
    gui/simple_widgets/pixmaplabel.h \
    gui/simple_widgets/quizcreatorwidget.h \
    gui/simple_widgets/titlewidget.h \
    logic/basicquestion.h \
    gui/mainwindow.h \
    logic/database/databasemanager.h \
    logic/database/quizdatabase.h \
    logic/database/quizstore.h \
    logic/multiquestion.h \
    logic/quizdatabase.h \
    logic/singlequestion.h \
    logic/singlequiz.h

FORMS += \
    gui/mainwindow.ui \
    gui/question_widgets/createquestionwidget.ui \
    gui/question_widgets/questionwidget.ui \
    gui/quiz_widgets/createquizwidget.ui \
    gui/quiz_widgets/quiziconwidget.ui \
    gui/quiz_widgets/quizrepresentationwidget.ui \
    gui/quiz_widgets/quizsummarywidget.ui \
    gui/quiz_widgets/singlequizwidget.ui \
    gui/simple_widgets/aboutdialog.ui \
    gui/simple_widgets/changepathdialog.ui \
    gui/simple_widgets/imagedialog.ui \
    gui/simple_widgets/mainmenu.ui \
    gui/simple_widgets/titlewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mainResources.qrc
