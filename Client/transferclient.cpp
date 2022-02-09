#include "transferclient.h"


TransferClient::TransferClient(QString host, qint16 port): host(host), port(port)
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


void TransferClient::sendPacket()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    QString message = "cc";

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    socket->write(packet);

}



void TransferClient::dataReceived()
{
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

    debug(message);
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
    QTimer::singleShot(30000, this, SLOT(reconnect()));

}
void TransferClient::debug(QString message) {
    qDebug().noquote().nospace() << message;
}
