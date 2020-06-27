#ifndef PIXMAPLABEL_H
#define PIXMAPLABEL_H

#include <QLabel>

class PixmapLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PixmapLabel(QWidget *parent = nullptr);
    /**
     * @brief getPixmap
     * @return originalPixmap
     */
    const QPixmap& getPixmap() const {return originalPixmap;}

public slots:
    void setPixmap(const QPixmap& pixmap);

private:
    QPixmap originalPixmap;

    void scalePixmap();
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // PIXMAPLABEL_H
