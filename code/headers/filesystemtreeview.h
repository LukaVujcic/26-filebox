#ifndef FILESYSTEMTREEVIEW_H
#define FILESYSTEMTREEVIEW_H
#include <qtreeview.h>
#include <QFileSystemModel>
#include <QString>
#include <QVector>
#include <QDir>
#include <QPair>
class FileSystemTreeView:public QTreeView
{
public:
    FileSystemTreeView(QWidget *parent=nullptr);
    QPair<QVector<QString>,QVector<QString>> getSelectedFiles() const;
};

#endif // FILESYSTEMTREEVIEW_H
