#include "pixmaplabel.h"
#include <QDebug>

PixmapLabel::PixmapLabel(QWidget *parent) : QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);
    this->setText(tr("Nie wybrano obrazu"));
}

void PixmapLabel::setPixmap(const QPixmap &pixmap)
{
    this->originalPixmap = pixmap;
    this->scalePixmap();
}

void PixmapLabel::scalePixmap()
{
    if(originalPixmap.isNull()){
        QLabel::setPixmap(this->originalPixmap);
        this->setMinimumSize(QLabel::minimumSizeHint());
        this->setText(tr("Nie wybrano obrazu"));
        return;
    }
    this->setMinimumSize(1,1);
    int w = this->width();
    int h = this->height();

    QLabel::setPixmap(this->originalPixmap.scaled(w,h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void PixmapLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    scalePixmap();
}
