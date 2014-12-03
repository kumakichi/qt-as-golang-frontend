#ifndef FRONTENDTHREADWRITER_H
#define FRONTENDTHREADWRITER_H

#include <QThread>
#include <qsharedmemory.h>
#include "msg_ring.h"

#define MAX_SHM_SIZE 5242880 // 1024*1024*5 -> 5M

class FrontendThreadWriter : public QThread
{
    Q_OBJECT

public:
    FrontendThreadWriter(ipc_ring *r, QObject *parent);
    ~FrontendThreadWriter();
    void run();

signals:
    void FoundBackendMsg();

private:
    bool work;
    ipc_ring *ring;
    QSharedMemory sharedMemory;

private slots:
    void exitThread();
};

#endif // FRONTENDTHREADWRITER_H
