#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDebug>
#include "server.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager(Server *server);
    void prepareSendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath);
public slots:
    void sendFile();
private:
    Server *server;
    void debug(QString message);
    QString currentHostPath;
    QTcpSocket *currentClient;
};

#endif // FILEMANAGER_H
