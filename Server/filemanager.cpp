#include "filemanager.h"

FileManager::FileManager(Server *server)
{
    this->server = server;
}

void FileManager::prepareSendFile(QTcpSocket *client, QString &hostPath, QString &clientOutputPath) {

    hostPath = hostPath.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    hostPath = hostPath.replace("<Current>", QCoreApplication::applicationDirPath());
    hostPath = hostPath.replace("<User>", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    QFileInfo hostInfo(hostPath);
    if(!hostInfo.exists() || !hostInfo.isFile()) {
        debug("Error, " + hostPath + " does not exist!");
        return;
    }

    QFileInfo clientFileInfo(clientOutputPath);
    if(clientFileInfo.fileName().isEmpty() || !clientFileInfo.fileName().contains(".")) {
        clientOutputPath = QDir::cleanPath(clientOutputPath + QDir::separator() + hostInfo.fileName());
    }
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();
    hostFile = new QFile(hostPath);

    if(hostFile->open(QFile::ReadOnly)){
        server->sendStringPacket(client, "prepareReceiveFile:" + clientOutputPath);
        debug("Sending file from " + hostPath + " to " + clientOutputPath + " ...");

        this->currentHostPath = hostPath;
        this->currentClient = client;
    }
    else {
        debug("Error while opening host file!");
        server->sendStringPacket(client, "error");
    }


}
void FileManager::debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}
void FileManager::sendFile() {

    connect(currentClient, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));

    byteToWrite = hostFile->size();
    totalSize = hostFile->size();
    QDataStream out(&outBlock, QIODevice::WriteOnly);

    out << qint64 (0) << qint64 (0);

    totalSize += outBlock.size();
    byteToWrite += outBlock.size();

    out.device()->seek(0);
    out << totalSize << qint64(outBlock.size());

    currentClient->write(outBlock);
}
void FileManager::goOnSend(qint64 numBytes)
{
    byteToWrite-= numBytes;
    outBlock = hostFile->read(qMin(byteToWrite, static_cast<qint64>(4*1024)));
    currentClient->write(outBlock);
    if(byteToWrite == 0) {
        debug("File sending completed!");
        hostFile->close();
        disconnect(currentClient, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));
    }
}
