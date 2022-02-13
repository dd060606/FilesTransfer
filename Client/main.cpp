#include <QCoreApplication>
#include "transferclient.h"
#include "qautostart.h"

void debug(QString message) {
    qDebug().noquote().nospace() << message;
}
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    if(argc <= 2) {
        debug("Syntax: filestransfer.exe <hostname> <port> <optional-args>");
        QTimer::singleShot(0,[](){
                 QCoreApplication::exit(0);
        });
        return a.exec();
    }
    debug("Welcome to FilesTransfer");

    QStringList args;
    bool autoStart = false;
    for(int i = 1;i < argc; i++) {
        if(QString(argv[i]).contains("startup")) {
            autoStart = true;
        }
        args << argv[i];
    }
    QAutoStart as(args);
    as.setAutostart(autoStart);


    debug("Connecting to host...");

    TransferClient client(argv[1],static_cast<qint16>(QString::fromStdString(argv[2]).toULongLong()));

    return a.exec();

}
