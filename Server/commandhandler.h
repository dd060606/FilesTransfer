#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QTextStream>
#include <QDebug>
#include <QThread>

class CommandHandler : public QThread {
    Q_OBJECT
public:
    CommandHandler();

private:
    QTextStream ts;

protected:
    void run();
private slots:
    void lineIsReady();
};

#endif // COMMANDHANDLER_H
