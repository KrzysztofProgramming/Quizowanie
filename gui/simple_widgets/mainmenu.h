#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

namespace Ui {
class MainMenu;
}
class QPushButton;
class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

    QPushButton *startButton();
    QPushButton *startCustomButton();
    QPushButton *creatQuizButton();
    QPushButton *manageButton();

    void hideExitButton();
    void showExitButton();

private:
    Ui::MainMenu *ui;
};

#endif // MAINMENU_H
