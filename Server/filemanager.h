#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include "server.h"

class FileManager
{
    Q_OBJECT
public:
    FileManager(Server *server);
    void sendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath);
private:
    Server *server;
};

#endif // FILEMANAGER_H
