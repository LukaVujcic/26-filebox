#include "filesystemtreeview.h"
#include <serverfilesbrowser.h>
FileSystemTreeView::FileSystemTreeView(QWidget *parent) : QTreeView(parent) {}
QPair<QVector<QString>, QVector<QString>> FileSystemTreeView::getSelectedFiles() const
{
      QModelIndexList selectedItems = selectionModel()->selectedRows();
      auto *modelFileSystem = dynamic_cast<QFileSystemModel *>(model());
      QVector<QString> folders;
      QVector<QString> files;
      for (auto &selected : selectedItems)
      {
            QFileInfo infoFile(modelFileSystem->filePath(selected));
            if (infoFile.isDir())
            {
                  folders.push_back(modelFileSystem->filePath(selected));
            }
            else if (infoFile.isFile())
            {
                  files.push_back(modelFileSystem->filePath(selected));
            }
      }
      return qMakePair(folders, files);
}
void FileSystemTreeView::setViewFolder(const QString &path)
{
      auto *model = new QFileSystemModel();
      auto oldModel = this->model();
      model->setRootPath(path);
      this->setModel(model);
      this->setRootIndex(model->index(path));
      if (oldModel != nullptr) delete oldModel;
}
void FileSystemTreeView::getServerFilesystem(TCPClient *socket, QString &username)
{
      QString unzipPath = "filesystem-" + username;
      QString pathFile = socket->fileSystemRequest();
      QDir unzipDir(unzipPath);
      if (unzipDir.exists())
      {
            unzipDir.removeRecursively();
      }
      ServerFilesBrowser sfb(pathFile);
      this->setViewFolder(sfb.getFolderPath(unzipPath));
}


