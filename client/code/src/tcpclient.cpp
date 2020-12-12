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
//TODO Implement folderTraversal
//void TCPClient::folderTraversal(QString rootFolderPath)
//{
//  QDirIterator it(rootFolderPath, QDirIterator::Subdirectories);
//  QDir rootFolder(rootFolderPath);

//  while (it.hasNext()) {
//      auto t=it.next();
//      this->sendMessage(t+"\n");
//      this->sendMessage(rootFolder.dirName()+t.right(t.length()-rootFolderPath.length())+"\n");
//      qDebug()<<t;
//     qDebug() << rootFolder.dirName()+t.right(t.length()-rootFolderPath.length());
//  }
//}
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
//TODO Implement sendAll
//void TCPClient::sendAll(const QVector<QString> &files,const QVector<QString>&folders)
//{

//    for(const auto& file: files){
//        //this->sendMessage("UPLOAD\r\n");
//        this->sendFile(file);
//    }
//}

void TCPClient::sendMessage(QString message)
{
    this->write(message.toStdString().c_str());
    this->flush();
    this->waitForBytesWritten(1000);
    //qDebug()<<this->bytesToWrite();
}

