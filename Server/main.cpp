#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <string>
#include "server.h"

const QString VERSION = "1.0";

void debug(QString message) {
    qDebug().noquote().nospace() << message;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    debug("Weclome to FilesTransfer " + VERSION + "\n");
    Server server;

    return a.exec();
}

