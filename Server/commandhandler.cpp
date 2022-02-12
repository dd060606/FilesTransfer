#include "commandhandler.h"

CommandHandler::CommandHandler(Server *server): fileManager(server)
{
    this->server = server;
    //Commands init
    this->addCommand("help", "", "Show all commands");
    this->addCommand("exit", "", "Exit the program");
    this->addCommand("list", "", "List all clients ready to share files");
    this->addCommand("disconnect", "<id>", "Disconnect client");
    this->addCommand("send", "<id> <\"hostPath\"> <\"clientOutputPath\">", "Send a file to a client");
    this->addCommand("debug-send-packet", "<id> <packet-value>", "Send a string packet to a client");
    this->addCommand("debug-mkdir", "<id> <path>", "Create a directory with command line - (debug)");


    QObject::connect(server, SIGNAL(cReadyToReceive()), &fileManager, SLOT(sendFile()));

}
void CommandHandler::debug(QString message) {
    qDebug().noquote().nospace() << message << "\n";
}
void CommandHandler::handleCommand(const QString &command)
{
    if(isCommandExists(command)){
        if (command.compare("exit", Qt::CaseInsensitive) == 0)
        {
            debug("Exiting...");
            QCoreApplication::quit();
        }
        else if (command.compare("help", Qt::CaseInsensitive) == 0)
        {
            debug("Commands: ");
            for(Command &cmd : this->commandsList) {
                debug("- " + cmd.command() + " " + cmd.syntax() + " Description: " + cmd.desc());
            }
        }
        else if (command.compare("list", Qt::CaseInsensitive) == 0)
        {
            if(this->server->getClients().length() == 0) {
                debug("No clients connected!");
                return;
            }
            debug("All clients: ");
            for(int i = 0; i < this->server->getClients().length();i++) {
                debug("- Id: " + QString::number(i) +" - "+ this->server->getClients().at(i)->peerAddress().toString());
            }
        }
        else if (command.startsWith("disconnect", Qt::CaseInsensitive)) {
            int id = getClientIdFromCommand(command);
            if(id != -1) {
                for(int i = 0; i < this->server->getClients().length(); i++) {
                    if(i == id) {
                        this->server->sendStringPacket(this->server->getClients().at(i), "stop");
                        return;
                    }
                }
                debug("Client " +QString::number(id)+" not found!");
            }
            else {
                showCommandSyntax(command);
            }
        }
        else if (command.startsWith("send", Qt::CaseInsensitive)) {
            QStringList splitCmd = command.split(" ");
            if(splitCmd.length() >= 4) {
                int id = getClientIdFromCommand(command);
                if(id != -1) {
                    for(int i = 0; i < this->server->getClients().length(); i++) {
                        if(i == id) {
                            QRegExp reg("\"([^\"]*)\"");
                            QStringList pathsList;
                            int pos = 0;
                            while ((pos = reg.indexIn(command, pos)) != -1)
                            {
                               pathsList << reg.cap(1);
                               pos += reg.matchedLength();
                            }
                            if(!pathsList.isEmpty()) {
                                if(pathsList.length() >= 2) {
                                    this->fileManager.prepareSendFile(this->server->getClients().at(i),pathsList[0], pathsList[1]);
                                }
                                else {
                                    debug("Wrong path format!");
                                    debug("Syntax: "+getCommandFromName(command).syntax());
                                }

                            }
                            else {
                                this->fileManager.prepareSendFile(this->server->getClients().at(i),splitCmd[2], splitCmd[3]);
                            }
                            return;
                        }
                    }
                    debug("Client " +splitCmd[1]+" not found!");
                }
                else {
                    showCommandSyntax(command);
                }
            }
            else {
                showCommandSyntax(command);
            }
        }
        else if (command.startsWith("debug", Qt::CaseInsensitive)) {
            this->handleDebugCommand(command);
        }
    }
    else {
        debug("Command not found!");
    }
}
void CommandHandler::handleDebugCommand(const QString &command) {
    if (command.startsWith("debug-send-packet", Qt::CaseInsensitive)) {
        int id = getClientIdFromCommand(command);
        if(id != -1) {
            QStringList splitCmd = command.split(" ");
            if(splitCmd.length() >= 3) {
                for(int i = 0; i < this->server->getClients().length(); i++) {
                    if(i == id) {
                        this->server->sendStringPacket(this->server->getClients().at(i), splitCmd[2]);
                        return;
                    }
                }
                debug("Client " +QString::number(id)+" not found!");
            }
            else {
                showCommandSyntax(command);
            }
        }
        else {
            showCommandSyntax(command);
        }
    }
    else if(command.startsWith("debug-mkdir", Qt::CaseInsensitive)) {
        int id = getClientIdFromCommand(command);
        if(id != -1) {
            QStringList splitCmd = command.split(" ");
            if(splitCmd.length() >= 3) {
                splitCmd.removeFirst();
                splitCmd.removeFirst();
                QString path = splitCmd.join(" ");
                for(int i = 0; i < this->server->getClients().length(); i++) {
                    if(i == id) {
                        this->server->sendStringPacket(this->server->getClients().at(i), "debug-mkdir:" + path);
                        return;
                    }
                }
                debug("Client " +QString::number(id)+" not found!");
            }
            else {
                showCommandSyntax(command);
            }
        }
        else {
            showCommandSyntax(command);
        }
    }

}

void CommandHandler::addCommand(QString name, QString syntax, QString desc) {
    if(isCommandExists(name)) {
        return;
    }
    this->commandsList << Command(name,syntax,desc);
}
bool CommandHandler::isCommandExists(const QString &name) {
    for (Command &cmdToCheck : this->commandsList  )
    {
        if(name.startsWith(cmdToCheck.command(), Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}
Command CommandHandler::getCommandFromName(const QString &name) {
    for (Command cmdToCheck : qAsConst(this->commandsList))
    {
        if(name.startsWith(cmdToCheck.command(), Qt::CaseInsensitive) ) {
            return cmdToCheck;
        }
    }
}
void CommandHandler::showCommandSyntax(const QString &name) {
    debug("Error, the syntax is: " + getCommandFromName(name).syntax());
}
int CommandHandler::getClientIdFromCommand(const QString &command) {
    QStringList splitCmd = command.split(" ");
    if(splitCmd.length() >= 2) {
        bool isNum;
        int id = splitCmd[1].toInt(&isNum);
        if (isNum) {
            return id;
        }
    }
    return -1;
}
