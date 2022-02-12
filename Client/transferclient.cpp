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
                sendStringPacket("debugMsg:folder created!");
            }
        }
        else {
            debug(message);
        }
    }
}
