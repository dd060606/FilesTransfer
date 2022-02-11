#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include "command.h"
#include "server.h"
#include "filemanager.h"

class CommandHandler : public QObject
{
    Q_OBJECT
public:
    CommandHandler(Server *server);
    void handleCommand(const QString &command);
private:
    void debug(QString message);
    void addCommand(QString command, QString syntax, QString desc);
    bool isCommandExists(const QString &name);
    Command getCommandFromName(const QString &name);
    void showCommandSyntax(const QString &name);
    void handleDebugCommand(const QString &command);
    int getClientIdFromCommand(const QString &command);

    QList<Command> commandsList;
    Server *server;
    FileManager fileManager;
};

#endif // COMMANDHANDLER_H
