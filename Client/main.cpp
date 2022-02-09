#include <QCoreApplication>
#include "transferclient.h"
#include <iostream>
#include <windows.h>


void debug(QString message) {
    qDebug().noquote().nospace() << message;
}
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    if(argc <= 2) {
        debug("Syntax: filestransfer.exe <hostname> <port>");
        QTimer::singleShot(0,[](){
                 QCoreApplication::exit(0);
        });
    }
    debug("Welcome to FilesTransfer");
    debug("Connecting to host...");
    TransferClient client(argv[1],static_cast<qint16>(QString::fromStdString(argv[2]).toULongLong()));

    return a.exec();

}
