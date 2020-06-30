#ifndef CHANGEPATHDIALOG_H
#define CHANGEPATHDIALOG_H

#include <QDialog>
#include <functional>

namespace Ui {
class ChangePathDialog;
}
QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE
class ChangePathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePathDialog(QWidget *parent = nullptr);
    ~ChangePathDialog();


    void setError(const QString& error);
    QString lastError();
    /**
     * @brief setDisplayPath this function only change lineEdit text;
     * @param path
     */
    void setDisplayPath(const QString& path);
    void setPath(const QString& path);
    /**
     * @brief getPath
     * @return absolute path selected by the user
     */
    const QString& getPath() const {return startPath;}
    QString getDisplayPath() const;
    void setSelectingFiles(bool value) {this->selectingFiles = value;}

    void setDescription(const QString& text);
    QString getDescription() const;

    inline bool isNoEmptyError() const {return noEmptyError;}
    inline void setNoEmptyError(bool value) {this->noEmptyError = value;}

    inline const QString& getErrorMessage() const {return errorMessage;}
    inline std::function<bool(const QString&)> getCheckingFunction() const {return checkingFunction;}

    inline void setErrorMessage(const QString& message) {this->errorMessage = message;}
    inline void setCheckingFunction(const std::function<bool(const QString&)>& function) {this->checkingFunction = function;}

    inline void setCommentFunction(const std::function<QString(const QString&)>& function) {this->commentFunction = function;}
    inline std::function<QString(const QString&)> getCommentFunction() const {return this->commentFunction;}


private slots:
    void onPathButtonClicked();
    void onAcceptButtonClicked();

private:
    Ui::ChangePathDialog *ui;
    QString startPath = "";
    QPushButton *okButton;
    QPushButton *cancelButton;

    std::function<bool(const QString&)> checkingFunction = nullptr;
    std::function<QString(const QString&)> commentFunction = nullptr;
    QString errorMessage;

    bool noEmptyError = true;
    bool selectingFiles = false;

    //default path checking, if commentFunction is nullptr
    void checkPath(const QString& path);

    // QDialog interface
public slots:
    virtual int exec() override;
};

#endif // CHANGEPATHDIALOG_H
