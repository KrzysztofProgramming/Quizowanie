#ifndef BUSYLABEL_H
#define BUSYLABEL_H

#include <QLabel>

class BusyLabel : public QLabel
{
    Q_OBJECT
public:
    static constexpr int defaultDotCount = 3;
    static constexpr int defaultInterval = 500;

    explicit BusyLabel(QWidget *parent = nullptr);
    inline int getMaxDotCount() const {return maxDotCount;}

    const QString& getText() const {return text;}
    int getInterval() const;
    void setInterval(const int& interval);
    void setMaxDotCount(const int& count){if(count > 0) this->maxDotCount = count;}

public slots:
    void setText(const QString& text) {this->text = text;}
private slots:
    void onTimer();
private:
    QTimer *timer;
    QString dottedString = "";
    int maxDotCount = defaultDotCount;
    QString text;

};

#endif // BUSYLABEL_H
