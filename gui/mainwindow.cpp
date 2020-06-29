#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simple_widgets/titlewidget.h"
#include "./logic/database/databasemanager.h"
#include "simple_widgets/changepathdialog.h"
#include "simple_widgets/aboutdialog.h"
#include "./logic/database/quizstore.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , dataDir(this->dataDirectory)
    , ui(new Ui::MainWindow)
    , pathDialog(new ChangePathDialog(this))
    , pathFile(new QFile(customQuizesFileDirectory))
    , aboutDialog(new AboutDialog(this))
{
    ui->setupUi(this);
    this->setWindowTitle("Quizowanie");
    loadFromDataDir();

    defaultQuizStore = new QuizStore(this, quizesDirectory);
    customQuizStore = new QuizStore(this, customQuizesDirectory);

    titlewidget = new TitleWidget(this);

    ui->centralWidget->addWidget(titlewidget);

    createActions();
    putTextToAboutDialog();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onChangingDirClicked()
{
    pathDialog->setPath(this->customQuizStore->getDir().absolutePath());
    pathDialog->setDescription(tr("Podczas przenoszenia wycinany jest cały folder z quizami a jego stara lokalizacja jest usuwana"));
    pathDialog->setNoEmptyError(false);
    pathDialog->setCommentFunction([=](const QString& path)->QString{
        if(QDir(this->customQuizesDirectory).absolutePath() == path){
            return "";
        }
        QDir dir(path);
        if(!dir.isEmpty()){
            return tr("Ten folder nie jest pusty!");
        }
        else if(DatabaseManager::isChildDirectory(this->customQuizesDirectory, dir)){
            return tr("Folder do którego przenosisz quizy nie może być w aktualnym folderze quizów");
        }
        else return tr("OK");
    });
    pathDialog->setCheckingFunction([=](const QString& path)->bool{
        Q_UNUSED(path);
        if(QDir(customQuizesDirectory).absolutePath() == path){
            return true;
        }
        QDir dir(path);
        if(!dir.isEmpty()){
            QMessageBox::critical(pathDialog, tr("Error"), tr("Folder do którego chcesz przenieść quizy musi być pusty!"));
            return false;
        }
        else if(DatabaseManager::isChildDirectory(this->customQuizesDirectory, dir)){
            QMessageBox::critical(pathDialog, tr("Error"), tr("Folder do którego przenosisz quizy nie może być w aktualnym folderze quizów"));
            return false;
        }
        return true;
    });

    int value = pathDialog->exec();
    if(value == QDialog::Accepted){
        if(this->customQuizStore->getDir().absolutePath()!= pathDialog->getPath()){
            this->customQuizesDirectory = QDir().relativeFilePath(pathDialog->getPath());

            if(this->savePathToFile() && this->customQuizStore->changeDir(customQuizesDirectory, true)){
                QMessageBox::information(this, tr("Sukces"), tr("Pomyślnie zmieniono ścieżkę"));
            }
            else{
                this->customQuizesDirectory = QDir().relativeFilePath(this->customQuizStore->getDir().path());
                this->savePathToFile();
                QMessageBox::critical(this, tr("Niepowodzenie"), tr("Nie udało się zmienić ścieżki"));
            }
        }
    }
}

void MainWindow::onAboutClicked()
{
    this->aboutDialog->exec();
}

void MainWindow::onAddQuizClicked()
{
    pathDialog->setPath("");
    pathDialog->setNoEmptyError(true);
    pathDialog->setDescription("Wybierz folder z quizem, zostanie on skopiowany do innych twoich quizów");
    pathDialog->setCommentFunction([=](const QString& path)->QString{
        QDir dir(path);
        if(!DatabaseManager::canMove(dir, this->customQuizStore->getDir()))
            return tr("Folder o takiej nazwie już istnieje w katalogu z quizami");
        else if(DatabaseManager::containsQuizDatabase(dir))
            return tr("OK");
        else
            return tr("To nie jest właściwy quiz");
    });

    pathDialog->setCheckingFunction([=](const QString& path)->bool{
        if(!DatabaseManager::canMove(path, this->customQuizStore->getDir())){
            QMessageBox::information(pathDialog, tr("Zły folder"), tr("Folder o takiej nazwie już istnieje w katalogu z quizami"));
            return false;
        }
        if(DatabaseManager::containsQuizDatabase(path))
            return true;
        QMessageBox::information(pathDialog, tr("Zły quiz"), tr("Ten folder nie zawiera quizu"));
        return false;
    });

    int value = pathDialog->exec();
    if(value == QDialog::Accepted){
        singleQuizPtr quiz = DatabaseManager::readQuizFromDir(pathDialog->getPath());
        if(quiz == nullptr){
            QMessageBox::critical(this, tr("ERROR"), tr("Nie udało się skopiować quizu, sprawdź czy na pewno jest poprawny"));
        }
        else{
            bool repetition = false;
            for(const singleQuizPtr& ptr: qAsConst(this->customQuizList)){
                if(quiz->getTitle() == ptr->getTitle()){
                    repetition = true;
                    break;
                }
            }
            if(repetition){
                QMessageBox::information(this, tr("Niepowodzenie"), tr("Już masz quiz o tytule: ") + quiz->getTitle());
            }

            else if(!DatabaseManager::copyDir(pathDialog->getPath(), this->customQuizesDirectory)){
                QMessageBox::critical(this, tr("Bład"), tr("Nie udało się skopowiać quizu"));
            }
            else{
                titlewidget->onQuizSave(quiz);
                QMessageBox::information(this, tr("Sukces"), tr("Pomyślnie władowano twój quiz"));
            }
        }

    }
}


void MainWindow::createActions()
{
    QMenu *menu = this->menuBar()->addMenu(tr("&Menu"));

    this->addQuizAction = menu->addAction(tr("&Dodaj quiz"), this, &MainWindow::onAddQuizClicked);
    this->changeDirAction = menu->addAction(tr("&Zmiana ścieżki quizów"), this, &MainWindow::onChangingDirClicked);
    this->aboutAction = menu->addAction(tr("&O programie"), this, &MainWindow::onAboutClicked);
}

void MainWindow::loadFromDataDir()
{
    //folder data
    if(!dataDir.exists()){
        dataDir.mkdir(dataDir.absolutePath());
    }

    //domyslne quizy w data/quizy
    if(!DatabaseManager::isQuizesDirectory(quizesDirectory)){
        QMessageBox::warning(this, tr("UWAGA"), tr("Nie znaleziono folderu z domyslnymi quizami, jeśli go usunąłeś, przywróć go aby móc z nich korzystać\n")
                             + QDir::toNativeSeparators(QDir(quizesDirectory).absolutePath()));
    }

    //lambda do wczytywania ścieżki custom quizów
    auto invalidFile = [=]()->QString{
        pathDialog->setNoEmptyError(true);
        pathDialog->setDescription("Zaznacz ścieżkę z twoimi quizami lub taką, gdzie mogą zostać stworzone");
        pathDialog->setCommentFunction([](const QString& path)->QString{
            if(DatabaseManager::isQuizesDirectory(path))
                return tr("OK");
            return tr("Nie zawiera poprawnych quizów");
        });

        pathDialog->setCheckingFunction([=](const QString& path)->bool{
            if(QDir(path).isEmpty()){
                int value= QMessageBox::warning(pathDialog, tr("UWAGA"), tr("Wybrany folder jest pusty będziesz musiał ponownie stworzyć swoje quizy, kontynuować?"),
                                                QMessageBox::Yes | QMessageBox::Cancel);
                if(value == QMessageBox::Yes) return true;
                else return false;
            }
            else if(!DatabaseManager::isQuizesDirectory(path)){
                int value= QMessageBox::warning(pathDialog, tr("UWAGA"), tr("Wybrany folder zawiera już inne pliki i nie zawiera quizów, będziesz musiał je stworzyć ponownie, kontynuować?"),
                                                QMessageBox::Yes | QMessageBox::Cancel);
                if(value == QMessageBox::Yes) return true;
                else return false;
            }
            return true;
        });

        if(pathDialog->exec() != QDialog::Accepted) exit(EXIT_SUCCESS);
        return QDir().relativeFilePath(pathDialog->getPath());
    };

    //zczytywanie ścieżki custom quizów i poprawiania w razie potrzeby
    QTextStream stream(this->pathFile);
    if(!pathFile->exists()){
        QMessageBox::warning(this, tr("UWAGA"), tr("Nie odnaleziono ścieżki do folderu z twoimi quizami, wskaż ją teraz"));

        customQuizesDirectory = invalidFile();
        if(!pathFile->open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zapisie pliku"));
            exit(EXIT_FAILURE);
        }
        stream<<customQuizesDirectory;
        pathFile->close();
    }
    else{
        if(!pathFile->open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zczytywaniu ścieżki"));
            exit(EXIT_FAILURE);
        }
        customQuizesDirectory = stream.readAll();
        pathFile->close();
        QDir customDir(customQuizesDirectory);
        if(!customDir.exists()){
            QMessageBox::warning(this, tr("UWAGA"), tr("Folder z twoimi quizami został usunięty, wskaż nowy, lub wyłącz program i go przywróć"));
            customQuizesDirectory = invalidFile();
            if(!pathFile->open(QFile::WriteOnly | QFile::Text | QFile::Truncate)){
                QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zapisywaniu śćieżki"));
                exit(EXIT_FAILURE);
            }
            stream<<customQuizesDirectory;
            pathFile->close();
        }
    }


}

bool MainWindow::savePathToFile()
{
    pathFile->setFileName(this->customQuizesFileDirectory);
    QTextStream stream(pathFile);

    if(!pathFile->open(QFile::WriteOnly | QFile::Text | QFile::Truncate)){
        return false;
    }

    stream<<this->customQuizesDirectory;
    pathFile->close();
    return true;
}

void MainWindow::putTextToAboutDialog()
{
    this->aboutDialog->setText(tr("Program \"Quizowanie\" jest to program pozwalający na rozwiązywanie gotowych quizów, "
                                  "jak również tworzenia i grania we własne. Posiada również możliwość zmiany ścieżki przechowywania"
                                  " swoich quizów oraz na dodanie innych, już gotowych.\n\n"
                                  "Autor programu: Krzysztof Konieczny"));
}
