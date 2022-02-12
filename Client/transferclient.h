#ifndef TRANSFERCLIENT_H
#define TRANSFERCLIENT_H
#include <QObject>
#include <QtNetwork>
#include <QCryptographicHash>
#include "AES/qaesencryption.h"

class TransferClient : public QObject
{
    Q_OBJECT
public:
      TransferClient(QString host, qint16 port);

  private slots:
      void sendStringPacket(QString message);
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
      void handlePacket(QString &message);
      bool isReceivingFile;
      QString currentFilePath;
      QByteArray inBlock;
      qint64 totalSize;
      qint64 byteReceived;
      QFile *file;
      QByteArray hashKey;
      QByteArray hashIV;



};

#endif // TRANSFERCLIENT_H
