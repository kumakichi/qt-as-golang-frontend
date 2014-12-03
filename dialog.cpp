#include "dialog.h"
#include "ui_dialog.h"
#include <QtWidgets>
#include <QtNetwork>
#include "parseconf.h"
#include "protocol.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog), sharedMemory("Inner Shared Memory")
{
    ui->setupUi(this);

    ShmInit();

    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(LoadImg()));

    ParseConf(&host, &port, &backend_name);
    ServerListen(host, port);
    connect(&server, SIGNAL(MsgBackToFront()), this, SLOT(UpdateUi()));
    connect(this, SIGNAL(FoundFrontendMsg()), &server, SLOT(receivedFrontendMsg()));

    ui->statusLabel->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n")
                             .arg(server.serverAddress().toString()).arg(server.serverPort()));

    backend = new BackEndThread(backend_name, this);
    backend->start();

    connect(this, SIGNAL(CleanAndExitProg()), backend, SLOT(terminate()));
    connect(this, SIGNAL(CleanAndExitProg()), &server, SLOT(receivedProgExitMsg()));
    connect(&server, SIGNAL(ReaderWriterAlreadyExited()), this, SLOT(DoExit()));
}

void Dialog::ShmInit()
{
    if (sharedMemory.isAttached())
        sharedMemory.detach();

    if (!sharedMemory.create(MAX_SHM_SIZE))
        if(sharedMemory.error() != QSharedMemory::AlreadyExists)
            QMessageBox::critical(this, "Shared Mem Dialog", "Unable to create shared memory segment :" + sharedMemory.errorString());

    if(!sharedMemory.attach())
        if (sharedMemory.error() != QSharedMemory::NoError)
            QMessageBox::critical(this, "Shared Mem Dialog", "Unable to attach shared memory segment :" + sharedMemory.error());
}

void Dialog::ServerListen(QString host, int port)
{
    QHostAddress addr;

    if(host == "")
        addr = QHostAddress::Any;
    else
        addr = QHostAddress(host);

    if (!server.listen(addr, port)) {
        QMessageBox::critical(this, tr("Threaded Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server.errorString()));
        GracefullyExit();
        ::exit(-1);
    }
}

Dialog::~Dialog()
{
    GracefullyExit();
    sharedMemory.deleteLater();
}

void Dialog::GracefullyExit()
{
    if(backend->isRunning())
        backend->terminate();

    if(sharedMemory.isAttached())
        sharedMemory.detach();

    if(server.isListening())
        server.close();
    delete ui;
}

void Dialog::UpdateTxtLabel()
{
    sharedMemory.lock();

    QString tmp;
    char *msg = (char *)sharedMemory.data() + HEADER_LEN;
    tmp = QString::fromStdString(msg);
    ui->ipcLabel->setText(tmp);

    memset(sharedMemory.data(),0,MAX_SHM_SIZE);
    sharedMemory.unlock();
}

void Dialog::UpdateUi()
{
    struct ipc_msg *ipc = (struct ipc_msg*)sharedMemory.data();

    switch (ipc->type) {
    case MSG_START:
        UpdateTxtLabel();
        break;
    case MSG_IMG:
        UpdateImgLabel();
        break;
    case MSG_LBL:
        UpdateTxtLabel();
        break;
    default:
        break;
    }
}

void Dialog::LoadImg()
{
    sharedMemory.lock();
    char *shm = (char*)sharedMemory.data();
    build_msg(shm, MSG_ASK_IMG);
    sharedMemory.unlock();
    emit FoundFrontendMsg();
}

void Dialog::UpdateImgLabel()
{
    QImage image;
    struct ipc_msg *ipc = (struct ipc_msg*)sharedMemory.data();

    sharedMemory.lock();
    image.loadFromData((const uchar *)ipc->data, ipc->total_len, "PNG");

    memset(sharedMemory.data(),0,MAX_SHM_SIZE);
    sharedMemory.unlock();

    ui->imgLabel->setPixmap(QPixmap::fromImage(image));
}

void Dialog::closeEvent(QCloseEvent *event)
{
    emit CleanAndExitProg();
}

void Dialog::DoExit()
{
    qApp->exit();
}
