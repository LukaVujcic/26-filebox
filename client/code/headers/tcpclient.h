#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QObject>
#include <QString>
#include <QTcpSocket>
//#include <filesystemtreeview.h>
class TCPClient : public QTcpSocket
{
      Q_OBJECT
     public:
      TCPClient(QString ip, quint16 port);
      void sendMessage(const QString& message);
      void sendAll(const QVector<QString>& files, const QVector<QString>& folders, const QString& serverPath = "");
      void downloadRequest(const QVector<QString>& remoteFiles, const QVector<QString>& remoteFolders,
                           const QString& localFolder, const QString& rootPath);
      void folderRequest(const QString& pathRemote, const QString& rootPath);
      void renameRequest(const QString& pathRemote, const QString& rootPath, const QString& newName);
      void pasteRequest(const QString& pathRemote);
      QString fileSystemRequest();
      void multiSelect(const QVector<QString>& files, const QVector<QString>& folders,const QString&message,const QString& user_folder);

     private:
      void sendFolder(QString rootFolderPath, const QString& serverPath);
      void sendFile(const QString& filePath);
      void uploadRequest(const QString& pathLocal, const QString& pathRemote);
      void newFolderRequest(const QString& pathRemote, const QString& name);
      void receiveFile(const QString& filePath);
     private slots:
      void connected();
      void disconnected();
     signals:
         void uploadFinished();
         void downloadFinished();
         void multiSelectFinished(const QString& operation);
         void newFolderFinished();
         void renameFinished();
         void pasteFinished();
};

#endif  // TCPCLIENT_H
