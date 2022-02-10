#include "server.h"

Server::Server(qint16 port) {
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port))
    {
        debug("Error while starting server: " + server->errorString());
    }
    else
    {
        debug("Listening on port " + QString::number(server->serverPort()));
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }

    messageSize = 0;
}

void Server::newConnection()
{

    QTcpSocket *newClient = server->nextPendingConnection();
    clients << newClient;
    connect(newClient, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    debug(newClient->peerAddress().toString() + " is ready to transfer files !");

}
void Server::dataReceived()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

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
    messageSize = 0;
}
void Server::clientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;
    debug(socket->peerAddress().toString() + " disconnected from the server");
    clients.removeOne(socket);

    socket->deleteLater();
}
void Server::sendStringPacket(QTcpSocket *client, const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    for (int i = 0; i < clients.size(); i++)
    {
        if(clients[i] == client) {
            clients[i]->write(packet);
        }
    }

}
void Server::debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}
QList<QTcpSocket *> Server::getClients() {
    return clients;
}
void Server::handlePacket(QString &message) {
    if(!message.isEmpty()) {
        if(message.startsWith("error:", Qt::CaseInsensitive)) {
            QStringList list = message.split(":");
            list.removeFirst();
            QString error = list.join(":");
            debug("[Client]: " +error);
        }
        if(message.compare("readyToReceive", Qt::CaseInsensitive) == 0) {
            emit cReadyToReceive();
        }
    }
}
