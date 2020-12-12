#include "tcpconnection.h"

TCPConnection::TCPConnection(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);
    qDebug() << this << "created";
    active();
}

TCPConnection::~TCPConnection()
{
    qDebug() << this << "destroyed";
}

int TCPConnection::idleTime()
{
    return activity.secsTo(QTime::currentTime());
}

void TCPConnection::quit()
{
    qDebug() << this << "Quit called, closing client";

    foreach(QTcpSocket *socket, sockets)
    {
        socket->close();
    }

    emit closed();
}

void TCPConnection::accept(qintptr descriptor)
{
    qDebug() << this << " accepting " << descriptor;

    QTcpSocket* socket = createSocket();

    if(!socket)
    {
        qWarning() << this << "could not find created socket!";
    }

    sockets.append(socket);

    if(!socket->setSocketDescriptor(descriptor))
    {
        qDebug() << this << " setSocketDescriptor failed " << descriptor;
        return;
    }

    qDebug() << this << " accepted " << descriptor;
    activity = QTime::currentTime();
    emit opened();
}

void TCPConnection::connected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << this << " connected";
    activity = QTime::currentTime();
    emit opened();
}

void TCPConnection::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << this << socket << " disconnected";

    sockets.removeAll(socket);
    socket->deleteLater();

    qDebug() << this << "Notify Runnable of closure";
    activity = QTime::currentTime();
    emit closed();
}

bool TCPConnection::is_upload_request(QByteArray& msg)
{
    return !QString(msg).compare(("UPLOAD\r\n"));
}

bool TCPConnection::is_new_folder_request(QByteArray& msg)
{
    return !QString(msg).compare(("NEW FOLDER"));
}

bool TCPConnection::is_cut_request(QByteArray& msg)
{
    return !QString(msg).compare(("CUT"));
}

bool TCPConnection::is_copy_request(QByteArray& msg)
{
    return !QString(msg).compare(("COPY"));
}

bool TCPConnection::is_paste_request(QByteArray& msg)
{
    return !QString(msg).compare(("PASTE"));
}

bool TCPConnection::upload_file(QTcpSocket *socket, QByteArray file_path)
{

}

void TCPConnection::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());

    qDebug()<<socket->ConnectedState;
    qDebug()<<"Enter readyRead!";
    if(!socket)
        return;

    socket->waitForReadyRead(1000);
    QByteArray REQUEST = socket->readLine(1000);
    qDebug()<<REQUEST;
    socket->waitForReadyRead(1000);
    QByteArray file_path = socket->readLine(1000);
    qDebug()<<file_path;
    if(is_upload_request(REQUEST))
    {
        qDebug()<<"Request: "<<REQUEST<<" file path: "<<file_path<<"\n";
        qDebug()<<"File content: "<<"\n";

        QFile f(QString(file_path).trimmed());

        //QFile f(QString("C:/Users/bozam/Desktop/preko_mreze.txt").trimmed());

        if(f.exists())
        {
            qDebug()<<"File Exists";
            return;
        }

        f.open( QIODevice::WriteOnly);

        const int chunckSize=1024;
        char *chunk=new char[chunckSize+1];
        int total=0;
        int bytesRead;
        while(1)
        {
            qDebug()<<socket->ConnectedState;
            socket->waitForReadyRead(1000);

            if (socket->bytesAvailable()>0)
                bytesRead=socket->read(chunk,chunckSize);
            else
            {   qDebug()<<"bytesRead: "<<bytesRead;
                qDebug()<<"ELSE!!!"<<socket->ConnectedState;
                break;
            }

            if (bytesRead>=0)
            {
                total+=bytesRead;
            }
            //qDebug()<<bytesRead;
            if (bytesRead<=0)
            {
                qDebug()<<"bytesRead: "<<bytesRead;
                break;
            }
            f.write(static_cast<const char *>(chunk),bytesRead);
            f.flush();
            f.waitForBytesWritten(-1);
            //f.flush()


        }
        socket->waitForReadyRead(1000);
        qDebug()<<socket->readLine(1000);
        //qDebug()<<"Total: "<<total;
       // qDebug()<<"Nesto";
        //qDebug()<<total<<"BYTES";
        qDebug()<<socket->ConnectedState;
        delete[] chunk;
        f.close();
        qDebug()<<"Pisanje se zavrsava"<<"\n";
//        qDebug()<<socket->ConnectedState;
        qDebug()<<socket->write("OK\r\n");
        socket->flush();
        socket->waitForBytesWritten(1000);

    }
    else if(is_new_folder_request(REQUEST))
    {
        qDebug() << "Creating new folder.... " << REQUEST;
    }
    else if(is_cut_request(REQUEST))
    {
        qDebug() << "Cutting folder(s)... " << REQUEST;
    }
    else if(is_copy_request(REQUEST))
    {
        qDebug() << "Copying folder(s)... " << REQUEST;
    }
    else
    {
        qDebug() << "Pasting folder(s)... "<< REQUEST<<"\n";
    }

    active();
}

void TCPConnection::bytesWritten(qint64 bytes)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << socket << " bytesWritten " << bytes;
    active();
}
//void TCPConnection::sendMessage(QTcpSocket *socket,QString message)
//{
//    //QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
//    socket->write(message.toStdString().c_str());
//    socket->flush();
//    socket->waitForBytesWritten();
//    //qDebug()<<this->bytesToWrite();
//}

void TCPConnection::stateChanged(QAbstractSocket::SocketState socketState)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << socket << " stateChanged " << socketState;
    active();
}

void TCPConnection::error(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << socket << " error " << socketError;
    active();
}

QTcpSocket *TCPConnection::createSocket()
{
    qDebug() << this << " creating socket";

    QTcpSocket *socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TCPConnection::connected);
    connect(socket, &QTcpSocket::disconnected, this, &TCPConnection::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TCPConnection::readyRead);
    connect(socket, &QTcpSocket::bytesWritten, this, &TCPConnection::bytesWritten);
    connect(socket, &QTcpSocket::stateChanged, this, &TCPConnection::stateChanged);
    //connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &TCPConnection::error);

    return socket;
}

void TCPConnection::active()
{
    qDebug() << this << "socket active";
    activity = QTime::currentTime();


}
