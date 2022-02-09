#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <iostream>
#include <stdio.h>
#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#include <windows.h>
#else
#include <QSocketNotifier>
#endif

class Console : public QObject
{
    Q_OBJECT;

public:
    Console();

    void run();

signals:
    void quit();

private:
#ifdef Q_OS_WIN
    QWinEventNotifier *m_notifier;
#else
    QSocketNotifier *m_notifier;
#endif

private slots:
    void readCommand();

};


#endif
