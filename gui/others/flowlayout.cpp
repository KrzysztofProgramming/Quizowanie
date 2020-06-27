#include "flowlayout.h"
#include <QWidget>

FlowLayout::FlowLayout(QWidget *parent) : QLayout(parent)
{

}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while((item = this->takeAt(0)) != nullptr){
        delete item->widget();
        delete item->layout();
        delete item;
    }
}

void FlowLayout::setHorizontalSpacing(int value)
{
    if(value>=0)
        horizontalSpacing = value;
}
void FlowLayout::setVerticalSpacing(int value)
{
    if(value>=0)
        verticalSpacing = value;
}

int FlowLayout::makeLayout(const QRect &rect, bool test) const
{
    QMargins margin = this->contentsMargins();
    QRect area = rect.adjusted(margin.left(), margin.top(), -margin.right(), -margin.bottom());
    int x = area.x();
    int y = area.y();
    int lineSize = 0; //line size in layout

    int nextX;
    int spaceX = this->getHorizontalSpacing();
    int spaceY = this->getVerticalSpacing();

    for(QLayoutItem *item: qAsConst(this->layoutItems)){
        QSize itemSize = item->sizeHint();
        nextX = x + itemSize.width() + spaceX;
        if(nextX - spaceX > area.right() && lineSize > 0 ){
            x = area.x();
            y = y + lineSize + spaceY;
            nextX = x + itemSize.width() + spaceX;
            lineSize = 0;
        }

        if(!test) item->setGeometry(QRect(x, y, itemSize.width(), itemSize.height()));

        x = nextX;
        lineSize = qMax(lineSize, item->sizeHint().height());
    }
    return y + lineSize - rect.y() + margin.bottom();
}

QSize FlowLayout::sizeHint() const
{
    return  this->minimumSize();
}

void FlowLayout::addItem(QLayoutItem * item)
{
    this->layoutItems.append(item);
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    if(index < this->layoutItems.size() && index >=0 ){
        return layoutItems.at(index);
    }
    return nullptr;
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if(index < this->layoutItems.size() && index >=0 ){
        return layoutItems.takeAt(index);
    }
    return nullptr;
}

int FlowLayout::count() const
{
    return this->layoutItems.size();
}


QSize FlowLayout::minimumSize() const
{
    QSize largestWidgetSize;
    for(const QLayoutItem *item: qAsConst(this->layoutItems)){
        largestWidgetSize = largestWidgetSize.expandedTo(item->minimumSize());
    }
    QMargins margin = this->contentsMargins();
    largestWidgetSize += QSize(margin.left() + margin.right(), margin.top() + margin.bottom());
    return largestWidgetSize;
}


void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    this->makeLayout(rect);
}


bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    return makeLayout(QRect(0,0,width,0), true);
}
