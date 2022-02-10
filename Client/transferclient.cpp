#include "transferclient.h"


TransferClient::TransferClient(QString host, qint16 port): host(host), port(port), isReceivingFile(false)
{

    socket = new QTcpSocket(this);
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connect()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    messageSize = 0;
    socket->abort();
    socket->connectToHost(host, port);
}


void TransferClient::sendStringPacket(QString message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    socket->write(packet);

}



void TransferClient::dataReceived()
{

    if(!this->isReceivingFile) {
        QDataStream in(socket);

        if (messageSize == 0)
        {
            if (socket->bytesAvailable() < (int)sizeof(quint16))
                 return;

            in >> messageSize;
        }

        if (socket->bytesAvailable() < messageSize)
            return;
        QString message;
        in >> message;

        this->handlePacket(message);
    }
    else {
        QByteArray line = socket->readAll();
        QFile target(currentFilePath);

        if (!target.open(QIODevice::WriteOnly | QIODevice::Append)) {
            this->sendStringPacket("error:Error while opening " + currentFilePath + " for write!");
            this->isReceivingFile = false;
            return;
        }
        target.write(line);
        this->isReceivingFile = false;
        target.close();
    }
}

void TransferClient::connect()
{
    debug("Ready to share files !");

}

void TransferClient::disconnect()
{
    debug("Disconnected !");
}
void TransferClient::reconnect() {
    messageSize = 0;
    socket->abort();
    socket->connectToHost(this->host, this->port);
}
void TransferClient::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            debug("Host not found !");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            debug("Connection refused !");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            debug( "Remote host closed!");
            break;
        default:
            debug( "Error" + socket->errorString());
    }
    this->isReceivingFile = false;
    QTimer::singleShot(30000, this, SLOT(reconnect()));

}
void TransferClient::debug(QString message) {
    qDebug().noquote().nospace() << message;
}
void TransferClient::handlePacket(QString &message) {
    if(!message.isEmpty()) {
        if(message.compare("stop", Qt::CaseInsensitive) == 0) {
            socket->disconnectFromHost();
            QCoreApplication::quit();
        }
        else if(message.startsWith("prepareReceiveFile:", Qt::CaseInsensitive)) {
            QStringList pathList = message.split(":");
            pathList.removeFirst();
            QString path = pathList.join(":");
            path = path.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            path = path.replace("<Current>", QCoreApplication::applicationDirPath());
            debug(path);
            if(!path.isEmpty()) {
                QFileInfo fileInfo(path);

                if(!fileInfo.exists()) {
                    QDir dir(fileInfo.dir());
                    bool success = dir.mkpath(dir.path());
                    if(!success) {
                        this->sendStringPacket("error:Error while creating path " + path);
                        return;
                    }
                }
                this->currentFilePath = path;
                debug(path);
                this->isReceivingFile = true;
                this->sendStringPacket("readyToReceive");

            }
        }
    }
}
