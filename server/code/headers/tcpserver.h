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

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QStringList fortunes;

private slots:
    void on_task_done();

};

#endif // TCPSERVER_H
