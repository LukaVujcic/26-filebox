#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTime>

class TCPConnection : public QObject
{
    Q_OBJECT
public:
    explicit TCPConnection(QObject *parent = nullptr);
    ~TCPConnection();
    int idleTime();



signals:
    void opened();
    void closed();

public slots:
    virtual void quit();
    virtual void accept(qintptr descriptor);
    virtual void connected();
    virtual void disconnected();
    virtual void readyRead();
    virtual void bytesWritten(qint64 bytes);
    virtual void stateChanged(QAbstractSocket::SocketState socketState);
    virtual void error(QAbstractSocket::SocketError socketError);

private:
    bool is_upload(QByteArray& msg);
    bool create_folder(QByteArray& msg);

protected:
    QList<QTcpSocket*> sockets;
    QTime activity;

    QTcpSocket* createSocket();
    void active();
};

#endif // TCPCONNECTION_H
