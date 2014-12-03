#include "frontendthreadreader.h"
#include <QtNetwork>
#include <QMessageBox>
#include "protocol.h"
#include "parseconf.h"

FrontendThreadReader::FrontendThreadReader(ipc_ring *r, int sockfd, QObject *parent)
    : QThread(parent), work(true), ring(r), sharedMemory("Inner Shared Memory")
{
    max_pkg_size = MaxPktSize();

    tcpSock = new QTcpSocket(this);
    if (!tcpSock->setSocketDescriptor(sockfd)) {
        emit error(tcpSock->error());
        return;
    }
    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(readFortune()));

    if(!sharedMemory.attach())
        if (sharedMemory.error() != QSharedMemory::NoError)
            QMessageBox::critical(NULL, "Shared Mem", "Unable to attach shared memory segment :" + sharedMemory.errorString());
}

FrontendThreadReader::~FrontendThreadReader()
{
    if(sharedMemory.isAttached())
        sharedMemory.detach();
}

void FrontendThreadReader::run()
{
    for(;;){
        QThread::sleep(1);
        if(false == work)
            break;
    }
}

void FrontendThreadReader::readFortune()
{
    QByteArray b;

    b =  tcpSock->read(max_pkg_size);
    ring_write(ring, b.data(),b.size());
}

void FrontendThreadReader::sendRequestToBackend() // TODO: split and send
{
    struct ipc_msg *msg;

    sharedMemory.lock();
    msg = (struct ipc_msg*)sharedMemory.data();
    tcpSock->write((char *)sharedMemory.data(),msg->len + HEADER_LEN);
    sharedMemory.unlock();
}

void FrontendThreadReader::exitThread()
{
    work=false;
}
