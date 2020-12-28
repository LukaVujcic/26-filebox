#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
#include <QDebug>
#include <QString>
#include <QObject>
#include <QDirIterator>
#include <QDir>
#include <QDateTime>
//#include <filesystemtreeview.h>
class TCPClient:public QTcpSocket
{
Q_OBJECT
public:
    TCPClient(QString ip,quint16 port);
    void sendMessage(const QString& message);
    void sendAll(const QVector<QString> &files,const QVector<QString>&folders,const QString& serverPath="");
    void downloadRequest(const QVector<QString>& remoteFiles, const QVector<QString> &remoteFolders, const QString &localFolder,const QString &rootPath);
    QString fileSystemRequest();
private:
    void folderTraversal(QString rootFolderPath,const QString& serverPath);
    void sendFile(const QString &filePath);
    void uploadRequest(const QString& pathLocal,const QString& pathRemote);
    void newFolderRequest(const QString& pathRemote);
    void sendFolder(const QString& path);
    void receiveFile(const QString& filePath);
private slots:
    void connected();
    void disconnected();
};

#endif // TCPCLIENT_H
