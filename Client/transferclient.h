#ifndef TRANSFERCLIENT_H
#define TRANSFERCLIENT_H
#include <QObject>
#include <QtNetwork>


class TransferClient : public QObject
{
    Q_OBJECT
public:
      TransferClient(QString host, qint16 port);

  private slots:
      void sendPacket();
      void dataReceived();
      void connect();
      void disconnect();
      void socketError(QAbstractSocket::SocketError error);
      void reconnect();

  private:
      QString host;
      qint16 port;
      QTcpSocket *socket;
      quint16 messageSize;
      void debug(QString message);


};

#endif // TRANSFERCLIENT_H