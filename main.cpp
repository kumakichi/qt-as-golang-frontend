#include "dialog.h"
#include <QtCore>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog dialog;
    dialog.show();
    dialog.setWindowTitle("IPC Demo (TCP)");

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return app.exec();
}
