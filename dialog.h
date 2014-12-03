#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <qsharedmemory.h>
#include "backendthread.h"
#include "msgexchanger.h"

#define MAX_SHM_SIZE 5242880 // 1024*1024*5 -> 5M

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void GracefullyExit();
    void ServerListen(QString host, int port);

private:
    Ui::Dialog *ui;
    MsgExchanger server;
    BackEndThread *backend;

    QString host;
    int port;
    QString backend_name;

    QSharedMemory sharedMemory;

    void ShmInit();
    void UpdateImgLabel();
    void UpdateTxtLabel();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void CleanAndExitProg();
    void FoundFrontendMsg();

private slots:
    void UpdateUi();
    void LoadImg();
    void DoExit();
};

#endif // DIALOG_H
