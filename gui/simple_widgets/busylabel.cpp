#include "busylabel.h"
#include <QTimer>

BusyLabel::BusyLabel(QWidget *parent) : QLabel(parent)
  , timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &BusyLabel::onTimer);
    this->timer->start(defaultInterval);
    QFont font = this->font();
    font.setPointSize(15);
    this->setFont(font);
    this->setText(tr("Ładowanie"));
    this->setAlignment(Qt::AlignCenter);
}

int BusyLabel::getInterval() const
{
    return this->timer->interval();
}

void BusyLabel::setInterval(const int &interval)
{
    this->timer->start(interval);
}

void BusyLabel::onTimer()
{
    if(dottedString.size() >= maxDotCount){
        dottedString = "";
    }
    else dottedString.append('.');
    QLabel::setText(this->text + dottedString);
}
