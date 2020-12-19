#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTime>
#include <QFile>
#include <QDir>
#include <QString>
#include <filesystem>

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
    bool is_upload_request(QByteArray& msg);
    bool is_new_folder_request(QByteArray& msg);
    bool is_cut_request(QByteArray& msg);
    bool is_copy_request(QByteArray& msg);
    bool is_paste_request(QByteArray& msg);
    bool is_delete_request(QByteArray& msg);
    bool is_register_request(QByteArray& msg);
    bool is_login_request(QByteArray& msg);
    bool checkUsername(QString& username, QFile &file);
    bool checkProfile(QString& username, QString& password, QFile &file);

protected:
    QList<QTcpSocket*> sockets;
    QTime activity;

    QTcpSocket* createSocket();
    void active();
};

#endif // TCPCONNECTION_H
