#include "filemanager.h"

FileManager::FileManager(Server *server)
{
    this->server = server;
}

void FileManager::prepareSendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath) {

    hostPath = hostPath.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    hostPath = hostPath.replace("<Current>", QCoreApplication::applicationDirPath());

    QFileInfo hostInfo(hostPath);
    if(!hostInfo.exists() || !hostInfo.isFile()) {
        debug("Error, " + hostPath + " does not exist!");
        return;
    }

    QFileInfo clientFileInfo(clientOutputPath);
    if(!clientFileInfo.isFile()) {
        clientOutputPath = QDir::cleanPath(clientOutputPath + QDir::separator() + hostInfo.fileName());
    }
    QFile file(hostPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        server->sendStringPacket(client, "prepareReceiveFile:" + clientOutputPath);
        debug("Sending file from " + hostPath + " to " + clientOutputPath + " ...");

        this->currentHostPath = hostPath;
        this->currentClient = client;
    }
    else {
        debug("Error while opening host file!");
        server->sendStringPacket(client, "error");
    }
    file.close();


}
void FileManager::debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}
void FileManager::sendFile() {
    QFile file(this->currentHostPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray q = file.readAll();
    this->currentClient->write(q);
}
