#include "commandhandler.h"

CommandHandler::CommandHandler() : ts(stdin)

{
}

void CommandHandler::lineIsReady() {
    QString line = ts.readLine();
    qDebug() << line << "\n";
}
void CommandHandler::run() {

    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    QString word;
    qtin >> word;
}
