#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDebug>
#include <QtGlobal>
#include "server.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager(Server *server);
    void prepareSendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath);
public slots:
    void sendFile();
    void goOnSend(qint64);
private:
    Server *server;
    void debug(QString message);
    QString currentHostPath;
    QTcpSocket *currentClient;

    QByteArray outBlock;
    qint64 byteToWrite;
    qint64 totalSize;
    QFile *hostFile;
};

#endif // FILEMANAGER_H
