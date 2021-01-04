#ifndef FILESYSTEMTREEVIEW_H
#define FILESYSTEMTREEVIEW_H
#include <qtreeview.h>
#include <tcpclient.h>
#include <QDir>
#include <QFileSystemModel>
#include <QPair>
#include <QString>
#include <QVector>
class FileSystemTreeView : public QTreeView
{
     public:
      FileSystemTreeView(QWidget *parent = nullptr);
      QPair<QVector<QString>, QVector<QString>> getSelectedFiles() const;
      void setViewFolder(const QString &path);
      void getServerFilesystem(TCPClient *socket, QString &username);
};

#endif  // FILESYSTEMTREEVIEW_H
