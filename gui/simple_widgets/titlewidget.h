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
    void onQuizSaved(singleQuizPtr quiz);
    void onQuizStartEdit(singleQuizPtr quiz);
    void onQuizRemove(singleQuizPtr quiz);
    void onQuizEdited(singleQuizPtr oldVersion, singleQuizPtr newVersion);

private:
    MainWindow *mainWindow;
    Ui::TitleWidget *ui;
    MainMenu *mainMenu;
    MultiQuizWidget* defaultQuizes;
    MultiQuizWidget* customQuizes;
    QuizCreatorWidget* creator;

};

#endif // TITLEWIDGET_H
