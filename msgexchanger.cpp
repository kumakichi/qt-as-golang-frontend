#include "msgexchanger.h"

MsgExchanger::MsgExchanger(QObject *parent)
    : QTcpServer(parent), threadReader(NULL), threadWriter(NULL)
{
    ring = ring_create(PKG_BUFF_NUM); //200*4096/1024 = 800k
}

MsgExchanger::~MsgExchanger()
{
    if(threadReader != NULL)
        threadReader->terminate();
    if(threadWriter != NULL)
        threadWriter->terminate();
}

void MsgExchanger::incomingConnection(qintptr sockfd)
{
    if(threadReader != NULL)
        threadReader->terminate();

    if(threadWriter != NULL)
        threadWriter->terminate();

    threadReader = new FrontendThreadReader(ring, sockfd, this);
    connect(this, SIGNAL(MsgFrontToBack()), threadReader, SLOT(sendRequestToBackend()));
    connect(threadReader, SIGNAL(finished()), threadReader, SLOT(deleteLater()));
    threadReader->start();

    threadWriter = new FrontendThreadWriter(ring, this);
    connect(threadWriter, SIGNAL(FoundBackendMsg()), this, SLOT(receivedBackendMsg()));
    threadWriter->start();

    connect(this, SIGNAL(ReaderWriterExit()), threadReader, SLOT(exitThread()));
    connect(this, SIGNAL(ReaderWriterExit()), threadWriter, SLOT(exitThread()));
    connect(this, SIGNAL(ReaderWriterExit()), this, SLOT(waitReaderWriterExit()));
}

void MsgExchanger::receivedBackendMsg()
{
    emit MsgBackToFront();
}

void MsgExchanger::receivedFrontendMsg()
{
    emit MsgFrontToBack();
}

void MsgExchanger::receivedProgExitMsg()
{
    emit ReaderWriterExit();
}

void MsgExchanger::waitReaderWriterExit()
{
    for(;;) {
        QThread::usleep(10);
        if(threadReader->isRunning() || threadWriter->isRunning())
            continue;
        break;
    }
    emit ReaderWriterAlreadyExited();
}
