#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>

#include <QCryptographicHash>
#include "AES/qaesencryption.h"


class Server : public QObject
{
    Q_OBJECT
public:
    Server(qint16 port);
    void sendStringPacket(QTcpSocket *client, const QString &message);
    QList<QTcpSocket *> getClients();


private slots:
    void newConnection();
    void dataReceived();
    void clientDisconnected();

signals:
    void cReadyToReceive();
private:
    QTcpServer *server;
    QList<QTcpSocket *> clients;
    quint16 messageSize;
    QByteArray hashKey;
    QByteArray hashIV;
    void debug(QString message);
    void handlePacket(QString &message);

};

#endif // SERVER_H
