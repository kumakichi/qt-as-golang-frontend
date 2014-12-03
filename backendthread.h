#ifndef BACKENDTHREAD_H
#define BACKENDTHREAD_H

#include <QThread>
#include <QProcess>

class BackEndThread : public QThread
{
    Q_OBJECT

public:
    BackEndThread(QString back, QObject *parent);
    ~BackEndThread();
    void run();
private:
    QProcess *q;
    QString backendProg;
};

#endif // BACKENDTHREAD_H
