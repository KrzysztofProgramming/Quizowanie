#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    QString getText() const;
    void setText(const QString& text);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
