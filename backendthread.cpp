#include "backendthread.h"
#include <QDir>

BackEndThread::BackEndThread(QString back, QObject *parent)
    : QThread(parent),backendProg(back)
{
}

void BackEndThread::run()
{
    QDir dir;
    QString cur;

    cur = dir.currentPath();

    q = new QProcess();
    q->start(cur + QDir::separator() + backendProg);
}

BackEndThread::~BackEndThread()
{
    q->terminate();
    q->waitForFinished();
    delete q;
}
