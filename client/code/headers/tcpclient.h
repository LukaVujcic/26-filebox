#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
#include <QDebug>
#include <QString>
#include <QObject>
#include <QDirIterator>
#include <QDir>
class TCPClient:public QTcpSocket
{
Q_OBJECT
public:
    TCPClient(QString ip,quint16 port);
    void sendMessage(QString message);
    //void sendAll(const QVector<QString> &files,const QVector<QString>&folders);

private:
    //void folderTraversal(QString rootFolderPath);
    void sendFile(const QString &filePath);
private slots:
    void connected();
    void disconnected();
};

#endif // TCPCLIENT_H
