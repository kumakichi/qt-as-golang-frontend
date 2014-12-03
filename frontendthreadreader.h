#ifndef FRONTEND_THREAD_H
#define FRONTEND_THREAD_H

#include <QThread>
#include <QTcpSocket>
#include <qsharedmemory.h>
#include "msg_ring.h"

#define MAX_SHM_SIZE 5242880 // 1024*1024*5 -> 5M

class FrontendThreadReader : public QThread
{
    Q_OBJECT

public:
    FrontendThreadReader(ipc_ring *r, int tcpSock, QObject *parent);
    ~FrontendThreadReader();
    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    bool work;
    int max_pkg_size;
    ipc_ring *ring;
    QTcpSocket *tcpSock;
    QSharedMemory sharedMemory;

private slots:
    void exitThread();
    void readFortune();
    void sendRequestToBackend();
};

#endif
