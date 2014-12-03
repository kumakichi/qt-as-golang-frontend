#include "frontendthreadwriter.h"
#include <QMessageBox>
#include "protocol.h"

FrontendThreadWriter::FrontendThreadWriter(ipc_ring *r, QObject *parent)
    : QThread(parent), work(true), ring(r), sharedMemory("Inner Shared Memory")
{
    if(!sharedMemory.attach())
        if (sharedMemory.error() != QSharedMemory::NoError)
            QMessageBox::critical(NULL, "Shared Mem", "Unable to attach shared memory segment :" + sharedMemory.errorString());
}

FrontendThreadWriter::~FrontendThreadWriter()
{
    if(sharedMemory.isAttached())
        sharedMemory.detach();
}

void FrontendThreadWriter::run()
{
    bool ok;
    int len;
    struct ipc_msg *msg;
    char *p, *shm = (char*)sharedMemory.data();

    for(;;){
        if(false == work)
            break;
        QThread::usleep(100);
        p = ring_get(ring, &len);
        if(NULL == p)
            continue;
        msg = (struct ipc_msg*)p;
        sharedMemory.lock();
        ok = parse_msg(shm, p);
        sharedMemory.unlock();

        if(ok && msg->lastPiece)
            emit FoundBackendMsg();
    }
}

void FrontendThreadWriter::exitThread()
{
    work = false;
}
