#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include "logic/singlequiz.h"

namespace Ui {
class TitleWidget;
}

class MainMenu;
class QPushButton;
class MainWindow;
class QuizAreaDisplay;
class QuizRepresentationWidget;
class SingleQuizWidget;
class MultiQuizWidget;
class QuizCreatorWidget;
class BusyLabel;

class TitleWidget : public QWidget
{
    Q_OBJECT
    friend class MainWindow;

public:
    explicit TitleWidget(MainWindow *parent);
    ~TitleWidget();

private slots:
    void startQuiz();
    void startCustomQuiz();
    void createCustomQuiz();
    void managaQuizes();
    void backToMenu(bool fromEditMode = false);
    void onQuizSave(singleQuizPtr quiz);
    void onQuizStartEdit(singleQuizPtr quiz);
    void onQuizRemove(singleQuizPtr quiz);
    void onQuizEdited(singleQuizPtr oldVersion, singleQuizPtr newVersion);
    void onQuizSavingCompleted(singleQuizPtr quiz);
    void onQuizSavingFailed(singleQuizPtr quiz);

private:
    MainWindow *mainWindow;
    Ui::TitleWidget *ui;
    MainMenu *mainMenu;
    MultiQuizWidget* defaultQuizes;
    MultiQuizWidget* customQuizes;
    QuizCreatorWidget* creator;
    BusyLabel *busyLabel;

    bool fromEditMode = false;;


};

#endif // TITLEWIDGET_H
