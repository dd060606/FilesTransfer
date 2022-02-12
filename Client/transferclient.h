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
      void prepareSendFile(QString &clientPath);
      void sendFile();
  public slots:
      void sendStringPacket(QString message);
  private slots:
      void dataReceived();
      void connect();
      void disconnect();
      void socketError(QAbstractSocket::SocketError error);
      void reconnect();
      void goOnSend(qint64);

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

      //File Sender
      QByteArray outBlock;
      qint64 byteToWrite;
      qint64 totalSizeToSend;
      QFile *clientFileToSend;
      QString clientPathToSend;



};

#endif // TRANSFERCLIENT_H
