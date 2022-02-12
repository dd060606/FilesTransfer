#include "filemanager.h"

FileManager::FileManager(TransferClient *client)
{
    this->client = client;
}

void FileManager::prepareSendFile(QTcpSocket *socket, QString &path, QString &hostOutputPath) {

    path = path.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    path = path.replace("<Current>", QCoreApplication::applicationDirPath());
    path = path.replace("<User>", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    QFileInfo hostInfo(path);
    if(!hostInfo.exists() || !hostInfo.isFile()) {
        debug("Error, " + path + " does not exist!");
        return;
    }

    QFileInfo clientFileInfo(hostOutputPath);
    if(clientFileInfo.fileName().isEmpty() || !clientFileInfo.fileName().contains(".")) {
        hostOutputPath = QDir::cleanPath(hostOutputPath + QDir::separator() + hostInfo.fileName());
    }
    byteToWrite = 0;
    totalSizeToSend = 0;
    outBlock.clear();
    clientFile = new QFile(path);

    if(clientFile->open(QFile::ReadOnly)){
        client->sendStringPacket("prepareReceiveFile:" + hostOutputPath);
        debug("Sending file from " + path + " to " + hostOutputPath + " ...");
        this->clientPath = path;
    }
    else {
        debug("Error while opening file!");
        client->sendStringPacket("error:Error while opening client file!");
    }


}
void FileManager::debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}
void FileManager::sendFile() {

    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));

    byteToWrite = clientFile->size();
    totalSize = clientFile->size();
    QDataStream out(&outBlock, QIODevice::WriteOnly);

    out << qint64 (0) << qint64 (0);

    totalSize += outBlock.size();
    byteToWrite += outBlock.size();

    out.device()->seek(0);
    out << totalSize << qint64(outBlock.size());

    socket->write(outBlock);
}
void FileManager::goOnSend(qint64 numBytes)
{
    byteToWrite-= numBytes;
    outBlock = clientFile->read(qMin(byteToWrite, static_cast<qint64>(4*1024)));
    socket->write(outBlock);
    if(byteToWrite == 0) {
        debug("File sending completed!");
        clientFile->close();
        disconnect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));
    }
}
