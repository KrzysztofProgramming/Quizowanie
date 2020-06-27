#ifndef QUIZICONWIDGET_H
#define QUIZICONWIDGET_H

#include <QFrame>
#include "logic/singlequiz.h"

namespace Ui {
class QuizIconWidget;
}
QT_BEGIN_NAMESPACE
class QPushButton;
class PixmapLabel;
QT_END_NAMESPACE
class QuizIconWidget : public QFrame
{
    Q_OBJECT

public:
    enum Mode{Display = 0, Edit = 1};

    constexpr static int defaultPrefferedWidth = 200;
    constexpr static int defaultPrefferedHeight = 200;
    explicit QuizIconWidget(QWidget *parent = nullptr);
    explicit QuizIconWidget(const singleQuizPtr& quiz, const QSize& size = QSize(defaultPrefferedWidth, defaultPrefferedHeight),
                            QWidget *parent = nullptr);
    ~QuizIconWidget();

    void setQuiz(const singleQuizPtr& quiz);
    void refreshUI();
    void setPrefferedSize(const QSize& size);


    QPushButton* startButton() const;
    QPushButton* editButton() const;
    QPushButton* removeButton() const;

    singleQuizPtr getQuiz() const {return quiz;}

    inline void setMode(Mode mode){if(this->mode != mode){this->mode = mode; emit modeChange(mode);}}
    inline Mode getMode() const {return this->mode;}

signals:
    void modeChange(Mode mode);

private slots:
    void onModeChanged(Mode mode);

private:
    Ui::QuizIconWidget *ui;
    singleQuizPtr quiz;
    QSize prefferedSize = QSize(defaultPrefferedWidth, defaultPrefferedHeight);
    Mode mode = Display;
    PixmapLabel *imgLabel;

    // QWidget interface
public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
};

#endif // QUIZICONWIDGET_H
