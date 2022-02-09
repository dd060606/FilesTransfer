#include "filemanager.h"

FileManager::FileManager(Server *server)
{
    this->server = server;
}

void FileManager::sendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath) {

}
