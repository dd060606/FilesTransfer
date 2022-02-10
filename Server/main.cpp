#include <QCoreApplication>
#include <QDebug>
#include <iostream>

#include "server.h"
#include "commandhandler.h"
#include "qconsolelistener.h"

const QString VERSION = "1.0";

void debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    debug("Weclome to FilesTransfer " + VERSION);
    int port = 55630;
    if(argc >= 2) {
        bool isNum;
        int newPort = QString(argv[1]).toInt(&isNum);
        if (isNum && newPort >= 0 && newPort <= 65535) {
            port = newPort;
        }
        else {
            debug("Invalid port! Default port set to " + QString::number(port));
        }
    }


    Server *server = new Server(static_cast<qint16>(port));
    CommandHandler commandHandler(server);
    QConsoleListener console;

    QObject::connect(&console, &QConsoleListener::newLine,
    [ &commandHandler](const QString &strNewLine) {
        commandHandler.handleCommand(strNewLine);

    });
    debug("Please enter a command: ");


    return a.exec();
}

