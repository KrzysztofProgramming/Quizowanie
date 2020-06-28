#ifndef QUIZSTORE_H
#define QUIZSTORE_H

#include <QObject>

class QuizStore : public QObject
{
    Q_OBJECT
public:
    explicit QuizStore(QObject *parent = nullptr);

signals:

};

#endif // QUIZSTORE_H
