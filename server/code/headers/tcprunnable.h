#ifndef TCPRUNNABLE_H
#define TCPRUNNABLE_H

#include "tcpconnection.h"

#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QEventLoop>
#include <QThread>

class TCPRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TCPRunnable(QObject *parent = nullptr);
    ~TCPRunnable();
    void run();
    int count();
    void setThreadedMode(bool value);

signals:
    void started();
    void finished();
    void quit();

public slots:
    void connecting(qintptr handle, TCPRunnable *runnable, TCPConnection* connection);
    void idle(int value);
    void closing();
    void opened();
    void closed();

protected:
    bool isThreaded;
    QList<TCPConnection*> connections;
    QEventLoop* loop;
    QReadWriteLock lock;

    TCPConnection *createConnection();
    void addSignals(TCPConnection* connection);
};

#endif // TCPRUNNABLE_H
