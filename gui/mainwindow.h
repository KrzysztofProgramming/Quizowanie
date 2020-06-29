#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./logic/singlequiz.h"
#include <QDir>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class TitleWidget;
class ChangePathDialog;
class DatabaseManager;
class AboutDialog;
class QuizStore;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const QString& getQuizesDirectory() const {return quizesDirectory;}
    const QString& getCustomQuizesDirectory() const {return customQuizesDirectory;}
    const QString& getDataDirectory() const {return dataDirectory;}
    const QString& getCustomQuizesFileDirectory() const {return customQuizesFileDirectory;}

    QuizStore* getDefaultQuizStore() const {return defaultQuizStore;}
    QuizStore* getCustomQuizStore() const {return customQuizStore;}


private slots:
    void onChangingDirClicked();
    void onAboutClicked();
    void onAddQuizClicked();

private:
    const QString quizesDirectory = "data/quizy";
    QString customQuizesDirectory = "wlasneQuizy";
    const QString dataDirectory = "data";
    const QString customQuizesFileDirectory = "data/customQuizesPath.txt";

    QList<singleQuizPtr> quizList;
    QList<singleQuizPtr> customQuizList;

    QDir dataDir;

    Ui::MainWindow *ui;
    TitleWidget *titlewidget;
    ChangePathDialog *pathDialog;
    QFile *pathFile;
    AboutDialog *aboutDialog;
    QuizStore *defaultQuizStore;
    QuizStore *customQuizStore;



    QAction *aboutAction;
    QAction *changeDirAction;
    QAction *addQuizAction;

    void createActions();
    void loadFromDataDir();
    bool savePathToFile();
    void putTextToAboutDialog();



};
#endif // MAINWINDOW_H
