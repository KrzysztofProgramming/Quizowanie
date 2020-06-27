#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ImageDialog;
}

QT_BEGIN_NAMESPACE
class QFileDialog;
class PixmapLabel;
QT_END_NAMESPACE

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(int width, int height,QWidget *parent = nullptr);
    explicit ImageDialog(QWidget *parent = nullptr);
    ~ImageDialog();
    void setPixmap(const QPixmap& pixmap);
    const QPixmap& getPixmap();

    void setDescription(const QString& description);
private slots:
    void selectImage();
private:
    Ui::ImageDialog *ui;
    QFileDialog *dialog;
    PixmapLabel *imgLabel;

    static QStringList filters;
};

#endif // IMAGEDIALOG_H
