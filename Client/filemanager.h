#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include "transferclient.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager(TransferClient *client);
    void prepareSendFile(QTcpSocket *server, QString &hostPath, QString &serverOutputPath);
public slots:
    void sendFile();
    void goOnSend(qint64);
private:
    TransferClient *client;
    void debug(QString message);
    QString clientPath;
    QTcpSocket *socket;

    QByteArray outBlock;
    qint64 byteToWrite;
    qint64 totalSize;
    QFile *clientFile;
};

#endif // FILEMANAGER_H
