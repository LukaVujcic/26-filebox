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
    return !QString(msg).compare(("UPLOAD"));
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

bool TCPConnection::is_register_request(QByteArray& msg)
{
    return !QString(msg).compare(("REGISTER\n"));
}

void TCPConnection::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    QByteArray data = socket->readLine();

    if(is_upload_request(data))
    {
        qDebug() << "Uploading content... " << data;
    }
    else if(is_new_folder_request(data))
    {
        qDebug() << "Creating new folder.... " << data;
    }
    else if(is_cut_request(data))
    {
        qDebug() << "Cutting folder(s)... " << data;
    }
    else if(is_copy_request(data))
    {
        qDebug() << "Copying folder(s)... " << data;
    }
    else if(is_register_request(data))
    {
        qDebug() << "Registration" << data;

        socket->waitForReadyRead(100);
        QString username = socket->readLine();
        socket->waitForReadyRead(100);
        QString password = socket->readLine();

        username.replace("\n", "");

        qDebug() << username << password;

        QFile file("./../users/users.txt");

        if(searchUsername(username, file))
        {
            socket->write("EXISTS");
            socket->waitForBytesWritten();
        } else
        {
            socket->write("CONTINUE");
            socket->waitForBytesWritten();
            file.seek(file.size());
            QString linija = "\n" + username + " " + password;
            file.write(linija.toStdString().c_str());
        }

        file.close();
    }
    else
    {
        qDebug() << "Pasting folder(s)... " << data;
    }

    qDebug() << this << socket << " TCPConnection::readyRead" << data;

    active();
}

bool TCPConnection::searchUsername(QString& username, QFile &file)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while (!file.atEnd()) {
        QString rec = file.readLine();
        rec.replace("\n", "");
        if(rec.contains(username)){
            return true;
        }
    }

    return false;
}

void TCPConnection::bytesWritten(qint64 bytes)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << socket << " bytesWritten " << bytes;
    active();
}

void TCPConnection::stateChanged(QAbstractSocket::SocketState socketState)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket)
        return;

    qDebug() << socket << " stateChanged " << socketState;
    qDebug() << socket << socket->readAll()<<"\n";
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

    connect(socket, &QTcpSocket::connected, this, &TCPConnection::connected, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::disconnected, this, &TCPConnection::disconnected, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::readyRead, this, &TCPConnection::readyRead, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::bytesWritten, this, &TCPConnection::bytesWritten, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::stateChanged, this, &TCPConnection::stateChanged, Qt::QueuedConnection);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &TCPConnection::error, Qt::QueuedConnection);

    return socket;
}

void TCPConnection::active()
{
    qDebug() << this << "socket active";
    activity = QTime::currentTime();
}
