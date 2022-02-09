#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>

class Command
{
public:
    Command(const QString &command, const QString &syntax, const QString &desc);
    QString command();
    QString syntax();
    QString desc();
private:
    QString m_command;
    QString m_syntax;
    QString m_desc;
};

#endif // COMMAND_H
