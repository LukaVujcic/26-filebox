#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcprunnable.h"

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QThreadPool>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QTimer>

class TCPServer : public QTcpServer
{
    Q_OBJECT

protected:
    ThreadMode threadMode = MODE_SINGLE;
    int maxConnections = 0;
    int connectionTimeout = 0;

    QList<TCPRunnable*> runnables;
    QTimer timer;

    virtual TCPRunnable *createRunnable();

    virtual void incomingConnection(qintptr handle);
    virtual void startRunnable(TCPRunnable *runnable);
    virtual void reject(qintptr handle);
    virtual void acceptSingle(qintptr handle);
    virtual void acceptPooled(qintptr handle);
    virtual void acceptThreaded(qintptr handle);
    virtual void startSingle();
    virtual void startPooled();
    virtual void startThreaded();
    virtual int connections();
    virtual void accept(qintptr handle, TCPRunnable *runnable);
};

#endif // TCPSERVER_H
