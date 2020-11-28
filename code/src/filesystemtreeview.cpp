#include "filesystemtreeview.h"
FileSystemTreeView::FileSystemTreeView(QWidget *parent):QTreeView(parent)
{

}
QPair<QVector<QString>,QVector<QString>> FileSystemTreeView::getSelectedFiles() const{

    QModelIndexList  selectedItems = selectionModel()->selectedRows();
    QFileSystemModel *modelFileSystem=dynamic_cast<QFileSystemModel*>(model());
    QVector<QString> folders;
    QVector<QString> files;
    for (auto &selected:selectedItems)
    {
        QFileInfo infoFile(modelFileSystem->filePath(selected));
        if (infoFile.isDir())
        {
            folders.push_back(modelFileSystem->filePath(selected));

        }
        else if(infoFile.isFile())
        {
            files.push_back(modelFileSystem->filePath(selected));
        }
    }
    return qMakePair(folders,files);
}
