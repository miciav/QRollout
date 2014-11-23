#ifndef MYGENERALTHREAD_H
#define MYGENERALTHREAD_H

#include <QThread>

class MyGeneralThread : public QThread
{
    Q_OBJECT
public:
    explicit MyGeneralThread(QObject *parent = 0);


signals:

public slots:

};

#endif // MYGENERALTHREAD_H
