#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simple_widgets/titlewidget.h"
#include "./logic/database/databasemanager.h"
#include "simple_widgets/changepathdialog.h"
#include "simple_widgets/aboutdialog.h"
#include "./logic/database/quizstore.h"
#include "./logic/database/quizdatabase.h"
#include "./gui/simple_widgets/busylabel.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , dataDir(this->dataDirectory)
    , ui(new Ui::MainWindow)
    , pathDialog(new ChangePathDialog(this))
    , pathFile(new QFile(customQuizesFileDirectory))
    , aboutDialog(new AboutDialog(this))
    , busyLabel(new BusyLabel(this))
{
    ui->setupUi(this);
    this->setWindowTitle("Quizowanie");
    loadFromDataDir();

    defaultQuizStore = new QuizStore(this, defaultDbPath);
    customQuizStore = new QuizStore(this, customDbPath);

    titlewidget = new TitleWidget(this);

    ui->centralWidget->addWidget(titlewidget);
    ui->centralWidget->addWidget(busyLabel);

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
    pathDialog->setSelectingFiles(false);
    pathDialog->setDescription(tr("Podczas przenoszenia wycinana jest baza danych i folder \\img"));
    pathDialog->setNoEmptyError(false);
    pathDialog->setCommentFunction([=](const QString& path)->QString{
        if(QDir(this->customDbPath).absolutePath() == path){
            return "";
        }
        QDir dir(path);
        if(!dir.isEmpty()){
            return tr("Ten folder nie jest pusty!");
        }
        else if(DatabaseManager::isChildDirectory(QFileInfo(this->customDbPath).dir(), dir)){
            return tr("Folder do którego przenosisz quizy nie może być w aktualnym folderze quizów");
        }
        else return tr("OK");
    });
    pathDialog->setCheckingFunction([=](const QString& path)->bool{
        Q_UNUSED(path);
        if(QDir(customDbPath).absolutePath() == path){
            return true;
        }
        QDir dir(path);
        if(!dir.isEmpty()){
            QMessageBox::critical(pathDialog, tr("Error"), tr("Folder do którego chcesz przenieść quizy musi być pusty!"));
            return false;
        }
        else if(DatabaseManager::isChildDirectory(QFileInfo(this->customDbPath).dir(), dir)){
            QMessageBox::critical(pathDialog, tr("Error"), tr("Folder do którego przenosisz quizy nie może być w aktualnym folderze quizów"));
            return false;
        }
        return true;
    });

    int value = pathDialog->exec();
    if(value == QDialog::Accepted){
        if(this->customQuizStore->getDir().absolutePath()!= pathDialog->getPath()){
            this->customDbPath = QDir().relativeFilePath(pathDialog->getPath() + '/' + this->getCustomQuizStore()->getDatabaseName());

            if(this->savePathToFile() && this->customQuizStore->changeDir(pathDialog->getPath(), true)){
                QMessageBox::information(this, tr("Sukces"), tr("Pomyślnie zmieniono ścieżkę"));
            }
            else{
                this->customDbPath = QDir().relativeFilePath(this->customQuizStore->getDir().path() + "/" + this->getCustomQuizStore()->getDatabaseName());
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
    pathDialog->setDescription("Wybierz bazę danych z innymi quizami zostaną one skopiowane do innych twoich quizów");
    pathDialog->setSelectingFiles(true);
    pathDialog->setCommentFunction([=](const QString& path)->QString{
        Q_UNUSED(path);
        return "";

    });

    pathDialog->setCheckingFunction([=](const QString& path)->bool{
        Q_UNUSED(path);
        return true;
    });

    int value = pathDialog->exec();
    if(value == QDialog::Accepted){
        QList<singleQuizPtr> *list = new QList<singleQuizPtr>(QuizDatabase::readQuizesFromDatabase(pathDialog->getPath()));
        if(list->isEmpty()){
            QMessageBox::critical(this, tr("ERROR"), tr("Nie skopiowano żadnych quizów, sprawdź czy wybrany plik na pewno jest poprawny"));
        }
        else{
            QStringList currentQuizesName = this->getCustomQuizStore()->getQuizesNames();
            QStringList *unAddedQuizesNames = new QStringList;
            QList<singleQuizPtr> quizesToRemove;
            for(const singleQuizPtr& ptr: qAsConst(*list)){
                if(currentQuizesName.contains(ptr->getTitle())){
                    unAddedQuizesNames->append(ptr->getTitle());
                    quizesToRemove.append(ptr);
                }
            }
            for(const singleQuizPtr& ptr: qAsConst(quizesToRemove)){
                list->removeAll(ptr);
            }

            if(!list->isEmpty()){

                QList<singleQuizPtr> *unAddedQuizes = new QList<singleQuizPtr>;
                int listSize = list->size();
                auto endIfFinished = [=](){
                    if(list->isEmpty()){
                        customQuizStore->disconnect(this);
                        delete list;
                        if(listSize == unAddedQuizes->size()){
                            QMessageBox::critical(this, tr("ERROR"), tr("Nie udało się dodać quizów"));

                        }
                        else if(unAddedQuizesNames->isEmpty() && unAddedQuizes->isEmpty()){
                            QMessageBox::information(this, tr("Sukces"), tr("Pomyslnie dodano quizy"));
                        }
                        else{
                            QString duplicatesTitles = "";
                            for(const QString& str: qAsConst(*unAddedQuizesNames)){
                                duplicatesTitles += "\n-\"" + str + '"';
                            }
                            if(!unAddedQuizesNames->isEmpty()) duplicatesTitles += '\n';

                            QString unknownReason = "";
                            for(const singleQuizPtr& ptr: qAsConst(*unAddedQuizes)){
                                unknownReason += "\n-\"" + ptr->getTitle() + '"';
                            }
                            if(!unAddedQuizesNames->isEmpty()) duplicatesTitles = tr("Nie dodano wszystkich Quizów;\n\nKonflikt tytułów:") + duplicatesTitles;
                            if(!unAddedQuizes->isEmpty()) unknownReason = tr("Nie dodano z nieznanych problemów:") + unknownReason;
                            QString message = duplicatesTitles + unknownReason;
                            QMessageBox::warning(this, tr("Zakończono operacje"), message);
                        }
                        delete unAddedQuizes;
                        delete unAddedQuizesNames;
                        this->ui->centralWidget->setCurrentWidget(titlewidget);
                        this->menuBar()->setEnabled(true);
                    }
                };
                connect(customQuizStore, &QuizStore::quizAdded, this, [=](singleQuizPtr quiz){
                    if(list->contains(quiz)){
                        list->removeAll(quiz);
                        endIfFinished();
                    }
                });
                connect(customQuizStore, &QuizStore::quizAdded, this, [=](singleQuizPtr quiz){
                    if(list->contains(quiz)){
                        list->removeAll(quiz);
                        unAddedQuizes->append(quiz);
                        endIfFinished();
                    }
                });
                this->getCustomQuizStore()->addQuizesAsync(*list);
                this->ui->centralWidget->setCurrentWidget(busyLabel);
                this->menuBar()->setEnabled(false);
            }
            else{
                delete list;
                if(!unAddedQuizesNames->isEmpty()) QMessageBox::critical(this, tr("Bład"), tr("Nie dodano żadnego quizów, gdyż masz już quizy o takich tytułach"));
                else QMessageBox::critical(this,tr("Zły plik"), tr("Z tego pliku nie można odczytać żadnych quizów"));
                delete unAddedQuizesNames;
            }
        }

    }
}


void MainWindow::createActions()
{
    QMenu *menu = this->menuBar()->addMenu(tr("&Menu"));

    this->addQuizAction = menu->addAction(tr("&Dodaj quizy"), this, &MainWindow::onAddQuizClicked);
    this->changeDirAction = menu->addAction(tr("&Zmiana ścieżki quizów"), this, &MainWindow::onChangingDirClicked);
    this->aboutAction = menu->addAction(tr("&O programie"), this, &MainWindow::onAboutClicked);
}

void MainWindow::loadFromDataDir()
{
    pathDialog->setSelectingFiles(false);
    //folder data
    if(!dataDir.exists()){
        dataDir.mkdir(dataDir.absolutePath());
    }

    //domyslne quizy w data/quizy
    if(!QFileInfo(defaultDbPath).exists()){
        QMessageBox::warning(this, tr("UWAGA"), tr("Nie znaleziono bazy danych z domyslnymi quizami, jeśli ją usunąłeś, przywróć ją aby móc z nich korzystać\n")
                             + QDir::toNativeSeparators(QDir(defaultDbPath).absolutePath()));
    }

    //lambda do wczytywania ścieżki custom quizów
    auto invalidFile = [=]()->QString{
        pathDialog->setNoEmptyError(true);
        pathDialog->setDescription(tr("Zaznacz ścieżkę z nowym katalogiem na twoje quizy"));
        pathDialog->setCommentFunction([](const QString& path)->QString{
            if(!QDir(path).isEmpty()){
                return tr("Katalog musi być pusty");
            }
            return "";
        });

        pathDialog->setCheckingFunction([=](const QString& path)->bool{
            if(!QDir(path).isEmpty()){
                QMessageBox::information(pathDialog, tr("Zajęty folder"), tr("Wybrany folder musi być pusty"));
                return false;
            }
            return true;
        });

        if(pathDialog->exec() != QDialog::Accepted) exit(EXIT_SUCCESS);
        return QDir().relativeFilePath(pathDialog->getPath());
    };

    //zczytywanie ścieżki custom quizów i poprawiania w razie potrzeby
    QTextStream stream(this->pathFile);
    if(!pathFile->exists()){
        QMessageBox::warning(this, tr("UWAGA"), tr("Nie odnaleziono folderu z twoimi quizami, wskaż nowy folder, gdzie będą mogły byś przechowywane"));

        customDbPath = invalidFile() + '/' + QFileInfo(customDbPath).fileName();
        if(!pathFile->open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zapisie pliku"));
            exit(EXIT_FAILURE);
        }
        stream<<customDbPath;
        pathFile->close();
    }
    else{
        if(!pathFile->open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zczytywaniu ścieżki"));
            exit(EXIT_FAILURE);
        }
        customDbPath = stream.readAll();
        pathFile->close();
        QFileInfo customDb(customDbPath);
        if(!customDb.exists()){
            QMessageBox::warning(this, tr("UWAGA"), tr("Baza danych z twoimi quizami została usunięay, wskaż nowy folder ich przechowywania lub wyłącz program i przywróć bazę"));
            customDbPath = invalidFile() + '/' + QFileInfo(customDbPath).fileName();;
            if(!pathFile->open(QFile::WriteOnly | QFile::Text | QFile::Truncate)){
                QMessageBox::critical(this, tr("ERROR"), tr("Błąd przy zapisywaniu śćieżki"));
                exit(EXIT_FAILURE);
            }
            stream<<customDbPath;
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

    stream<<this->customDbPath;
    pathFile->close();
    return true;
}

void MainWindow::putTextToAboutDialog()
{
    this->aboutDialog->setText(tr("Program \"Quizowanie\" jest to program pozwalający na rozwiązywanie gotowych quizów, "
                                  "jak również tworzenia i grania we własne. Posiada również możliwość zmiany ścieżki przechowywania"
                                  " swoich quizów oraz dodanie innych, już gotowych.\n\n"
                                  "Autor programu: Krzysztof Konieczny"));
}
