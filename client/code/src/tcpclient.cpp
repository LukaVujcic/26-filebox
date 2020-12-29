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
    const int chunckSize=1024*1024;
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
    delete[] chunk;
    file.close();

}
void TCPClient::folderTraversal(QString rootFolderPath,const QString& serverPath)
{
  QDirIterator it(rootFolderPath,QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
  QDir rootFolder(rootFolderPath);
  newFolderRequest(rootFolder.dirName());
  while (it.hasNext()) {
      auto currentEntry=it.next();
      QString pathLocal=currentEntry;
      QString pathRemote=serverPath+rootFolder.dirName()+currentEntry.right(currentEntry.length()-rootFolderPath.length());
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
void TCPClient::receiveFile(const QString& filePath)
{
    QFile f(QString(filePath).trimmed());

    if(f.exists())
    {
        qDebug()<<"File Exists";
        return;
    }

    if (!f.open(QIODevice::WriteOnly)){
        qDebug()<<"open";
        return;
    }
    this->waitForReadyRead(-1);
    QString fileSizeStr=this->readLine(3000);
    bool flag=0;
    qint64 bytesFile=fileSizeStr.trimmed().toLongLong(&flag,10);
    const int chunckSize=1024*1024*10;
    char *chunk=new char[chunckSize+1];
    int bytesRead;
    int total=0;
    while(1)
    {
        if (total>=bytesFile){
            break;
        }
        this->waitForReadyRead(-1);
        if (this->bytesAvailable()>0)
            bytesRead=this->read(chunk,chunckSize);
        else
        {   qDebug()<<"bytesRead: "<<bytesRead;
            qDebug()<<"ELSE!!!"<<this->ConnectedState;
            break;
        }
        total+=bytesRead;
        if (bytesRead<=0)
        {
            //qDebug()<<"bytesRead: "<<bytesRead;
            break;
        }
        f.write(static_cast<const char *>(chunk),bytesRead);
        f.flush();
        f.waitForBytesWritten(-1);
        //f.flush()
    }
    delete[] chunk;
    f.close();
    qDebug()<<"Pisanje se zavrsava"<<"\n";
    //        qDebug()<<socket->ConnectedState;
    // qDebug()<<this->write("OK\r\n");
    // this->flush();
    // this->waitForBytesWritten(1000);
}

QString TCPClient::fileSystemRequest()
{
    QString pathFile="filesystem.zip";
    QFile zip(pathFile);
    if (zip.exists())
    {
        zip.remove();
    }
    this->sendMessage("FILESYSTEM\r\n");
    this->receiveFile(pathFile);
    //this->sendMessage("Ok\r\n");
    return pathFile;
}
void TCPClient::sendAll(const QVector<QString> &files,const QVector<QString>&folders,const QString& destPath)
{
    QString serverPath="";
    if (destPath.compare(""))
    {
        serverPath=destPath;
    }
    serverPath=serverPath+"/";
    for(const auto& file: files){
        QFileInfo fileInfo(file);
        QString fileName = fileInfo.fileName();
        this->uploadRequest(file,serverPath+fileName);
        qDebug()<<serverPath+fileName;
        this->waitForReadyRead(-1);
        qDebug() <<this->readLine(1000);

    }
    for (const auto& folder:folders)
    {
        folderTraversal(folder,serverPath);
    }
}

void TCPClient::downloadRequest(const QVector<QString>& remoteFiles, const QVector<QString> &remoteFolders, const QString &localFolder,const QString &rootPath)
{
        QVector<QString> selected;
        selected=remoteFolders;
        for (const auto& file:remoteFiles){
            selected.push_back(file);
        }
        this->sendMessage("DOWNLOAD\r\n");
        this->sendMessage((QString::number(selected.size())+"\r\n").toLocal8Bit().data());
        for (const auto& selectItem: qAsConst(selected))
        {
            auto itemPath=selectItem.right(selectItem.length()-rootPath.length());
            qDebug()<<itemPath;
            this->sendMessage(itemPath+"\r\n");
        }
        auto timeInMSeconds=QString::number(QDateTime::currentMSecsSinceEpoch());
        this->receiveFile(localFolder+"/downloaded"+timeInMSeconds+".zip");
}
void TCPClient::sendMessage(const QString &message)
{
    this->write(message.toStdString().c_str());
    this->flush();
    this->waitForBytesWritten();
    //qDebug()<<this->bytesToWrite();
}

