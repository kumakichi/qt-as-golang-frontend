#ifndef UIMSGSERVER_H
#define UIMSGSERVER_H

#include <QTcpServer>
#include "frontendthreadreader.h"
#include "frontendthreadwriter.h"
#include "msg_ring.h"

#define PKG_BUFF_NUM 200

class MsgExchanger : public QTcpServer
{
    Q_OBJECT

public:
    MsgExchanger(QObject *parent = 0);
    ~MsgExchanger();

signals:
    void MsgBackToFront();
    void MsgFrontToBack();
    void ReaderWriterExit();
    void ReaderWriterAlreadyExited();

private:
    ipc_ring *ring;
    FrontendThreadReader *threadReader;
    FrontendThreadWriter *threadWriter;

protected:
    void incomingConnection(qintptr socketDescriptor);

private slots:
    void receivedBackendMsg();
    void receivedFrontendMsg();
    void receivedProgExitMsg();
    void waitReaderWriterExit();
};

#endif // UIMSGSERVER_H
