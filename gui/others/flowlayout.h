#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>

class FlowLayout : public QLayout
{
    Q_OBJECT
public:
    explicit FlowLayout(QWidget *parent = nullptr);
    virtual ~FlowLayout();


signals:

private:
    QList<QLayoutItem*> layoutItems;
    int verticalSpacing = 0;
    int horizontalSpacing = 0;
    QSize prefferedSize = QSize(0,0);


    int makeLayout(const QRect& rect, bool test = false) const;
    // QLayoutItem interface
public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSize() const override;
    virtual void setGeometry(const QRect& rect) override;

    // QLayout interface
public:
    virtual void addItem(QLayoutItem * item) override;
    virtual QLayoutItem *itemAt(int index) const override;
    virtual QLayoutItem *takeAt(int index) override;
    virtual int count() const override;

    inline int getVerticalSpacing() const {return this->verticalSpacing;}
    void setVerticalSpacing(int value);

    inline int getHorizontalSpacing() const {return this->verticalSpacing;}
    void setHorizontalSpacing(int value);

    // QLayoutItem interface
public:
    virtual bool hasHeightForWidth() const override;
    virtual int heightForWidth(int) const override;
};

#endif // FLOWLAYOUT_H
