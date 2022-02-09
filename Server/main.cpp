#include <QCoreApplication>
#include <QDebug>

#include "server.h"

#include "qconsolelistener.h"

const QString VERSION = "1.0";

void debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    debug("Weclome to FilesTransfer " + VERSION);
    QConsoleListener console;
    QObject::connect(&console, &QConsoleListener::newLine,
    [&a](const QString &strNewLine) {
        qDebug() << "Echo :" << strNewLine;
        // quit
        if (strNewLine.compare("q", Qt::CaseInsensitive) == 0)
        {
            debug("Good bye!");
            a.exit(0);
        }
    });
    debug("Please enter a command: ");


    return a.exec();
}

