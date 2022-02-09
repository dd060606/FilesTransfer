#include "command.h"

Command::Command(const QString &command, const QString &syntax, const QString &desc):
    m_command(command), m_syntax(syntax), m_desc(desc)  {

}

QString Command::command() {
    return m_command;
}
QString Command::syntax() {
    return m_syntax;
}
QString Command::desc() {
    return m_desc;
}
