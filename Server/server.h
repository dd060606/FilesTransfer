#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    void sendToAll(const QString &message);

private slots:
    void newConnection();
    void dataReceived();
    void clientDisconnected();

private:
    QTcpServer *server;
    QList<QTcpSocket *> clients;
    quint16 messageSize;
    void debug(QString message);

};

#endif // SERVER_H
