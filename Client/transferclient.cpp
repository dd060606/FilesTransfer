#include "transferclient.h"


TransferClient::TransferClient(QString host, qint16 port): host(host), port(port), isReceivingFile(false)

{

    socket = new QTcpSocket(this);

    hashKey = QCryptographicHash::hash(QString("1v7cltiQ9Jf17pIV4znCaof4Bkh5OpVn").toLocal8Bit(), QCryptographicHash::Sha256);
    hashIV = QCryptographicHash::hash(QString("RZZ5TCGkgM73r57VHd8WsWZV2OcKPJ7g").toLocal8Bit(), QCryptographicHash::Md5);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connect()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    messageSize = 0;

    inBlock.clear();
    byteReceived = 0;
    totalSize = 0;

    socket->abort();
    socket->connectToHost(host, port);
}


void TransferClient::sendStringPacket(QString message)
{

    //Encode packet
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray encodePacketMessage = encryption.encode(message.toLocal8Bit(), hashKey, hashIV);

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);


    out << (quint16) 0;
    out << encodePacketMessage;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    socket->write(packet);

}



void TransferClient::dataReceived()
{
    QDataStream in(socket);

    if(!this->isReceivingFile) {

        if (messageSize == 0)
        {
            if (socket->bytesAvailable() < (int)sizeof(quint16))
                 return;

            in >> messageSize;
        }

        if (socket->bytesAvailable() < messageSize)
            return;

        //Decode packet
        QByteArray encodedText;
        in >> encodedText;

        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
        QByteArray decodeText = encryption.decode(encodedText, hashKey, hashIV);
        QString decodedString = QString(encryption.removePadding(decodeText));
        this->handlePacket(decodedString);
    }
    else {
        if (byteReceived == 0)
        {

            in >> totalSize >> byteReceived;
            file = new QFile(currentFilePath);
            file->open(QFile::WriteOnly);
            debug("Downloading file from host to " + currentFilePath + " ...");
        }
        else
        {
            inBlock = socket->readAll();
            byteReceived += inBlock.size();
            file->write(inBlock);
            file->flush();
        }
        if (byteReceived == totalSize)
        {
            inBlock.clear();
            byteReceived = 0;
            totalSize = 0;
            this->isReceivingFile = false;
            file->close();
            debug("File successfully downloaded!");
            sendStringPacket("fileDownloaded");
        }
    }
    messageSize = 0;
}

void TransferClient::connect()
{
    debug("Ready to share files !");

}

void TransferClient::disconnect()
{
    debug("Disconnected !");
}
void TransferClient::reconnect() {
    messageSize = 0;
    socket->abort();
    socket->connectToHost(this->host, this->port);
}
void TransferClient::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            debug("Host not found !");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            debug("Connection refused !");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            debug( "Remote host closed!");
            break;
        default:
            debug( "Error" + socket->errorString());
    }
    this->isReceivingFile = false;
    QTimer::singleShot(30000, this, SLOT(reconnect()));

}
void TransferClient::debug(QString message) {
    qDebug().noquote().nospace() << message;
}
void TransferClient::handlePacket(QString &message) {
    if(!message.isEmpty()) {
        if(message.compare("stop", Qt::CaseInsensitive) == 0) {
            socket->disconnectFromHost();
            QCoreApplication::quit();
        }
        else if(message.startsWith("prepareReceiveFile:", Qt::CaseInsensitive)) {
            QStringList pathList = message.split(":");
            pathList.removeFirst();
            QString path = pathList.join(":");
            path = path.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            path = path.replace("<Current>", QCoreApplication::applicationDirPath());
            path = path.replace("<User>", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

            if(!path.isEmpty()) {
                QFileInfo fileInfo(path);

                if(!fileInfo.exists()) {
                    QDir dir(fileInfo.dir());
                    dir.mkpath(dir.path());
                }
                this->currentFilePath = path;
                this->isReceivingFile = true;
                this->sendStringPacket("readyToReceive");
            }
        }
        else if (message.startsWith("debug-mkdir:", Qt::CaseInsensitive)) {
            QString path = message.split("debug-mkdir:")[1].replace("/", "\\");
            QProcess process;
            process.start("cmd.exe /c mkdir " + path);
            process.waitForFinished();
            QString output = process.readAllStandardOutput();
            QString error = process.readAllStandardError();
            if(!output.isEmpty()) {
                sendStringPacket("debugMsg:" + output);
            }
            if(!error.isEmpty()) {
                sendStringPacket("debugMsg:" + error);
            }
            if(error.isEmpty() && output.isEmpty()) {
                sendStringPacket("debugMsg:Directory created!");
            }
        }
        else if (message.startsWith("get:", Qt::CaseInsensitive)) {
            prepareSendFile(message.split("get:")[1]);
        }
        else if (message.compare("readyToReceive", Qt::CaseInsensitive) == 0) {
            sendFile();
        }
        else {
            debug(message);
        }
    }
}

void TransferClient::prepareSendFile(QString &clientPath) {

    clientPath = clientPath.replace("<Desktop>", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    clientPath = clientPath.replace("<Current>", QCoreApplication::applicationDirPath());
    clientPath = clientPath.replace("<User>", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    QFileInfo clientInfo(clientPath);
    if(!clientInfo.exists() || !clientInfo.isFile()) {
        debug("Error, " + clientPath + " does not exist!");
        this->sendStringPacket("error:Error, " + clientPath + " does not exist!");
        return;
    }

    byteToWrite = 0;
    totalSizeToSend = 0;
    outBlock.clear();
    clientFileToSend = new QFile(clientPath);

    if(clientFileToSend->open(QFile::ReadOnly)){
        debug("Sending file from " + clientPath + " to host ...");
        sendStringPacket("readyToSend");
        this->clientPathToSend = clientPath;
    }
    else {
        debug("Error while opening file!");
        this->sendStringPacket("error:Error while opening client file!");
        return;
    }


}
void TransferClient::sendFile() {
    QObject::connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));

    byteToWrite = clientFileToSend->size();
    totalSize = clientFileToSend->size();
    QDataStream out(&outBlock, QIODevice::WriteOnly);

    out << qint64 (0) << qint64 (0);

    totalSize += outBlock.size();
    byteToWrite += outBlock.size();

    out.device()->seek(0);
    out << totalSize << qint64(outBlock.size());

    socket->write(outBlock);
}
void TransferClient::goOnSend(qint64 numBytes)
{
    byteToWrite-= numBytes;
    outBlock = clientFileToSend->read(qMin(byteToWrite, static_cast<qint64>(4*1024)));
    socket->write(outBlock);
    if(byteToWrite == 0) {
        debug("File sending completed!");
        clientFileToSend->close();
        QObject::disconnect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));
    }
}
