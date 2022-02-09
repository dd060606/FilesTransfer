#include <QCoreApplication>
#include <QDebug>

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

    Server *server = new Server();
    CommandHandler commandHandler(server);
    QConsoleListener console;

    QObject::connect(&console, &QConsoleListener::newLine,
    [&a, &commandHandler](const QString &strNewLine) {
        commandHandler.handleCommand(strNewLine);

    });
    debug("Please enter a command: ");


    return a.exec();
}

