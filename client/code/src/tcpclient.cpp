#include "tcpclient.h"

TCPClient::TCPClient(QString ip,quint16 port):
    QTcpSocket()
{
    connect(this, SIGNAL(connected()),this, SLOT(connected()));
    connect(this, SIGNAL(disconnected()),this, SLOT(disconnected()));
    this->connectToHost(ip,port);
    if (!this->waitForConnected())
    {
        qDebug()<<"Time out! Connection not established!";
        return;
    }
}

void TCPClient::connected()
{
    qDebug() << "connected...";
}

void TCPClient::disconnected()
{
    qDebug() << "disconnected...";

}
void TCPClient::sendFile(const QString &filePath){
    QFile file(filePath);
    qDebug()<<filePath;
    if(!file.open(QIODevice::ReadOnly))
        return ;
    const int chunckSize=1024;
    char *chunk=new char[chunckSize+1];
    while(true)
    {
        int bytesRead=file.read(chunk,chunckSize);
        this->write(static_cast<const char *>(chunk),bytesRead);
        this->waitForBytesWritten();
        if (bytesRead==0)
        {
            break;
        }
    }
    delete chunk;
    file.close();

}
void TCPClient::folderTraversal(QString rootFolderPath)
{
  QDirIterator it(rootFolderPath,QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
  QDir rootFolder(rootFolderPath);
  newFolderRequest(rootFolder.dirName());
  while (it.hasNext()) {
      auto currentEntry=it.next();
      QString pathLocal=currentEntry;
      QString pathRemote=rootFolder.dirName()+currentEntry.right(currentEntry.length()-rootFolderPath.length());
      QFileInfo fileInfo(pathLocal);
      if (fileInfo.isDir())
      {
          newFolderRequest(pathRemote);
      }
      else
      {
          uploadRequest(pathLocal,pathRemote);
      }
  }
}
void TCPClient::uploadRequest(const QString &pathLocal, const QString &pathRemote)
{
    this->sendMessage("UPLOAD\r\n");
    this->sendMessage(pathRemote+"\r\n");
    this->sendFile(pathLocal);
}

void TCPClient::newFolderRequest(const QString &pathRemote)
{
    this->sendMessage("NEW FOLDER\r\n");
    this->sendMessage(pathRemote+"\r\n");
}
void TCPClient::sendAll(const QVector<QString> &files,const QVector<QString>&folders)
{

    for(const auto& file: files){
        QFileInfo fileInfo(file);
        QString fileName = fileInfo.fileName();
        this->uploadRequest(fileName,fileName);
    }
    for (const auto& folder:folders)
    {
        folderTraversal(folder);
    }
}
void TCPClient::sendMessage(const QString &message)
{
    this->write(message.toStdString().c_str());
    this->flush();
    this->waitForBytesWritten();
}

