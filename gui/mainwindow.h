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
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    inline QList<singleQuizPtr>& getQuizes() {return quizList;}
    inline const QList<singleQuizPtr>& getQuizes() const {return quizList;}

    inline QList<singleQuizPtr>& getCustomQuizes() {return customQuizList;}
    inline const QList<singleQuizPtr>& getCustomQuizes() const {return customQuizList;}

    DatabaseManager* getCustomDbManager() const {return customDbManager;}

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
    DatabaseManager *dbManager;
    DatabaseManager *customDbManager;
    ChangePathDialog *pathDialog;
    QFile *pathFile;
    AboutDialog *aboutDialog;



    QAction *aboutAction;
    QAction *changeDirAction;
    QAction *addQuizAction;

    void loadAllQuizes();
    void createActions();
    void loadFromDataDir();
    bool savePathToFile();
    void putTextToAboutDialog();



};
#endif // MAINWINDOW_H
