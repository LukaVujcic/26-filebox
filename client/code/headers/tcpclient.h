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
    void sendMessage(const QString& message);
    void sendAll(const QVector<QString> &files,const QVector<QString>&folders);


private:
    void folderTraversal(QString rootFolderPath);
    void sendFile(const QString &filePath);
    void uploadRequest(const QString& pathLocal,const QString& pathRemote);
    void newFolderRequest(const QString& pathRemote);
    void sendFolder(const QString& path);
private slots:
    void connected();
    void disconnected();
};

#endif // TCPCLIENT_H
