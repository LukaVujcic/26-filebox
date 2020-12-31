#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcprunnable.h"

#include <QDebug>
#include <QNetworkInterface>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QTimer>

class TCPServer : public QTcpServer {
    Q_OBJECT
  public:
    explicit TCPServer(QObject *parent = nullptr);
    ~TCPServer() override;

    enum ThreadMode { MODE_SINGLE = 0, MODE_POOLED = 1, MODE_THREADED = 2 };

    void setMaxThreads(int value);
    void setMaxConnections(int value);
    void setConnectionTimeout(int value);
    void setMode(ThreadMode value);
    void listen(const QHostAddress &address, quint16 port);
    void close();

    QStringList getAddresses();

  signals:
    void connecting(
        qintptr handle, TCPRunnable *runnable, TCPConnection *connection);
    void closing();
    void idle(int value);

  public slots:
    void started();
    void finished();
    void timeout();

  protected:
    ThreadMode threadMode = MODE_POOLED;
    int maxConnections = 4;
    int connectionTimeout = 0;

    QList<TCPRunnable *> runnables;
    QTimer timer;

    virtual TCPRunnable *createRunnable();

    void incomingConnection(qintptr handle) override;
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
