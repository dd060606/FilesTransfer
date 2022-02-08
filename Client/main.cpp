#include <QCoreApplication>
#include "transferclient.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc <= 2) {
        qDebug().noquote().nospace() << "Syntax: filestransfer.exe <hostname> <port>";
        QCoreApplication::quit();
    }
    TransferClient client(argv[1],static_cast<qint16>(QString::fromStdString(argv[2]).toULongLong()));
    return a.exec();
}
