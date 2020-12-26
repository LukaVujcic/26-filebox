#include "tcpconnection.h"
#include <fstream>
#include <QFileInfo>
#include <QDir>

namespace fs = std::filesystem;

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

bool TCPConnection::is_download_request(QByteArray& msg)
{
    return !QString(msg).compare(("DOWNLOAD\r\n"));
}

bool TCPConnection::is_filesystem_request(QByteArray& msg)
{
    return !QString(msg).compare(("FILESYSTEM\r\n"));
}

bool TCPConnection::is_upload_request(QByteArray& msg)
{
    return !QString(msg).compare(("UPLOAD\r\n"));
}

bool TCPConnection::is_new_folder_request(QByteArray& msg)
{
    return !QString(msg).compare(("NEW FOLDER\r\n"));
}

bool TCPConnection::is_cut_request(QByteArray& msg)
{
    return !QString(msg).compare(("CUT\r\n"));
}

bool TCPConnection::is_copy_request(QByteArray& msg)
{
    return !QString(msg).compare(("COPY\r\n"));
}

bool TCPConnection::is_paste_request(QByteArray& msg)
{
    return !QString(msg).compare(("PASTE\r\n"));
}

bool TCPConnection::is_delete_request(QByteArray& msg)
{
    return !QString(msg).compare(("DELETE\r\n"));
}

bool TCPConnection::is_register_request(QByteArray& msg)
{
    return !QString(msg).compare(("REGISTER\r\n"));
}

bool TCPConnection::is_login_request(QByteArray& msg)
{
    return !QString(msg).compare(("LOGIN\r\n"));
}

void display_files_in_folder(QString current_path, QString original_path, Zipper& zip, QString user)
{
    QDir dir(current_path);

    if(dir.isEmpty()){
        char* dir_path = (dir.canonicalPath().right((dir.canonicalPath().size() - original_path.size())) + "/").toLocal8Bit().data();
        dir_path = (user + static_cast<QString>(dir_path)).toLocal8Bit().data();
        zip.add(dir_path, zip.SaveHierarchy);
        return;
    }

    foreach(auto name, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst))
    {
        if(name.isDir()){
            display_files_in_folder(name.filePath(), original_path, zip, user);
        }
        else{
            char* file_path = name.filePath().right(name.filePath().size() - original_path.size()).toLocal8Bit().data();
            file_path = (user  + static_cast<QString>(file_path)).toLocal8Bit().data();
            zip.add(file_path, zip.SaveHierarchy);
        }
     }
}

void get_files_and_folders(QString current_path, QString& server_path, Zipper& zip)
{
    QFileInfo file_info(current_path);

    if(file_info.isFile()){
       std::ifstream input1(file_info.filePath().toLocal8Bit().data());
       char* file_path = server_path.toLocal8Bit().data();
       zip.add(input1, file_path, zip.SaveHierarchy);
       //free(file_path);

       return;
    }

   QDir dir(file_info.filePath());

   if(dir.isEmpty()){
       char* dir_path = (server_path + "/").toLocal8Bit().data();
       zip.add(dir_path, zip.SaveHierarchy);
       //free(dir_path);

       return;
   }

   foreach(auto name, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst))
   {
       QString new_path = server_path + "/" + name.baseName();
       get_files_and_folders(name.filePath(), new_path, zip);
   }

}

void TCPConnection::sendFile(QString filePath){
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QFile file(filePath);
    //qDebug()<<filePath;
    if(!file.open(QIODevice::ReadOnly))
        return ;
    const int chunckSize=1024*1024;
    char *chunk=new char[chunckSize+1];
    int total=0;
    qint64 fileSize=file.size();
    //qDebug()<<"fileSize: "<<fileSize;
    auto message=(QString::number(fileSize)+"\r\n");
    //qDebug()<<"Poruka"<<(message+"\r\n").toLocal8Bit().data();
    socket->write((message+"\r\n").toLocal8Bit().data());
    socket->flush();
    socket->waitForBytesWritten(-1);
    while(true)
    {
        int bytesRead=file.read(chunk,chunckSize);
        socket->write(static_cast<const char *>(chunk),bytesRead);
        socket->waitForBytesWritten();
        if (bytesRead==0)
        {
            break;
        }
        total+=bytesRead;
    }
    delete[] chunk;
    file.close();
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
    //qDebug()<<REQUEST;

    if(is_download_request(REQUEST)){
        qDebug()<<"FROM DOWNLOAD: "<<REQUEST<<"\n";

        bool  conversion_successful = true;

        socket->waitForReadyRead(1000);
        int num_of_files = socket->readLine(1000).toInt(&conversion_successful);
        qDebug()<<"Broj linija: "<<num_of_files<<"\n";

        Zipper zipper("ziptest.zip");

        if(!conversion_successful){
            qDebug()<<"Expected number of lines!"<<"\n";
            return;
        }

        QString server_path;
        QString server_user_path = "../users/admin/";

        while(num_of_files--){
            socket->waitForReadyRead(1000);
            QString path= socket->readLine(1000);

            server_path = "../users/admin/" + path.trimmed();
            qDebug()<<"Cela putanja: "<<server_path<<"\n";

            QFileInfo f1(server_path);
            QString name = f1.baseName();

            get_files_and_folders(server_path, name, zipper);
        }

        zipper.close();

        sendFile("ziptest.zip");
        QFile zip("ziptest.zip");
        zip.remove();
    }

    else if(is_filesystem_request(REQUEST))
    {
//        const char* user_path = "/home/luka/Desktop/FileBox-repo/26-filebox/server/users/admin";

//        Zipper zipper("/home/luka/Desktop/FileBox-repo/26-filebox/server/users/ziptest.zip");
//        display_files_in_folder(user_path, user_path, zipper, "filesystem");
//        zipper.close();
//        sendFile("/home/luka/Desktop/FileBox-repo/26-filebox/server/users/ziptest.zip");
//        QFile zip("/home/luka/Desktop/FileBox-repo/26-filebox/server/users/ziptest.zip");
//        zip.remove();
        /*TODO username*/
        const char* user_path = "../users/admin";
        QDir dir(user_path);
        Zipper zipper("../users/ziptest.zip");

        display_files_in_folder(dir.absolutePath(), dir.absolutePath(), zipper, "filesystem");
        zipper.close();
        sendFile("../users/ziptest.zip");
        QFile zip("../users/ziptest.zip");
        zip.remove();
    }

    else if(is_upload_request(REQUEST))
    {
        socket->waitForReadyRead(1000);
        QByteArray file_path = socket->readLine(1000);
        qDebug()<<file_path;

        qDebug()<<"Request: "<<REQUEST<<" file path: "<<file_path<<"\n";
        qDebug()<<"File content: "<<"\n";
        /*TODO Srediti putanje*/
        QFile f(QString(file_path).trimmed());

        //QFile f(QString("C:/Users/bozam/Desktop/preko_mreze.txt").trimmed());

        /*if(f.exists())
        {
            qDebug()<<"File Exists";
            return;
        }*/

        f.open(QIODevice::WriteOnly | QIODevice::Truncate);

        const int chunckSize=1024*1024;
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

        //qDebug()<<"Total: "<<total;
        //qDebug()<<"Nesto";
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

        QDir dir_path(R"(./../server)");
        dir_path.mkdir("new folder");
    }
    else if(is_cut_request(REQUEST))
    {
        qDebug() << "Cutting folder(s)... " << REQUEST;

        const fs::path source {R"(D:\C++\Qt5)"};
        const fs::path destination {R"(C:\Users\Darko i Marko\Documents)"};

        try
        {
            const auto copy_options = fs::copy_options::recursive;
            const fs::path filename = source.filename();
            const fs::path new_path = destination / filename;

            // qDebug() << "Copying: " << source.filename() << " to " << new_path.filename();

            fs::create_directory(new_path);
            fs::copy(source, new_path, copy_options);
            fs::remove_all(source);
        }
        catch(const std::exception& e)
        {
            qDebug() << e.what();
        }
    }
    else if(is_copy_request(REQUEST))
    {
        qDebug() << "Copying folder(s)... " << REQUEST;
    }
    else if(is_paste_request(REQUEST))
    {
        qDebug() << "Pasting folder(s)... " << REQUEST;

        socket->waitForReadyRead(1000);
        QString destination_path = socket->readLine(1000);

        qDebug() << destination_path;

        for(const auto &source: selected_files)
        {
            const fs::path destination {destination_path.toStdString()};

            try
            {
                const fs::path file_name = source.filename();
                const fs::path new_path = destination / file_name;

                const auto copy_options = fs::copy_options::update_existing
                                        | fs::copy_options::recursive
                                        | fs::copy_options::copy_symlinks;

                if(fs::is_directory(source))
                {
                    fs::create_directory(new_path);
                }

                fs::copy(source, new_path, copy_options);
            }
            catch (const std::exception& e)
            {
                qDebug() << e.what();
            }
        }

        // PROVERA
        selected_files.clear();
    }



    else if(is_register_request(REQUEST))
    {
        qDebug() << "Registration" << REQUEST;

        socket->waitForReadyRead(100);
        QString username = socket->readLine();
        socket->waitForReadyRead(100);
        QString password = socket->readLine();

        username.replace("\n", "");

        qDebug() << username << password;

        QFile file("./../users/users.txt");

        if(checkUsername(username, file))
        {
            socket->write("EXISTS");
            socket->waitForBytesWritten();
        }
        else
        {
            socket->write("CONTINUE");
            socket->waitForBytesWritten();
            file.seek(file.size());
            QString line = "\n" + username + " " + password;
            file.write(line.toStdString().c_str());
            QDir folder("./../users");
            folder.mkdir(username);
        }

        file.close();
    }
    else if(is_login_request(REQUEST))
    {
        qDebug() << "Login" << REQUEST;

        socket->waitForReadyRead(100);
        QString username = socket->readLine();
        socket->waitForReadyRead(100);
        QString password = socket->readLine();

        username.replace("\n", "");

        QFile file("./../users/users.txt");

        if(checkProfile(username, password, file))
        {
            socket->write("CONTINUE");
            socket->waitForBytesWritten();
        }
        else
        {
            socket->write("ERROR");
            socket->waitForBytesWritten();
        }

        file.close();

    }
    else
    {
        qDebug() << "Delete folder(s)... "<< REQUEST<<"\n";

        socket->waitForReadyRead(1000);
        QString file_path = socket->readLine(1000);

        QDir dir_path(file_path);

        // PROVERA
        if(dir_path.exists())
            dir_path.removeRecursively();
    }

    active();
}

bool TCPConnection::checkUsername(const QString& username, QFile &file)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while (!file.atEnd())
    {
        QString rec = file.readLine();
        rec.replace("\n", "");
        QStringList list = rec.split(" ");
        if(!list[0].compare(username))
        {
            return true;
        }
    }

    return false;
}

bool TCPConnection::checkProfile(const QString& username, const QString& password, QFile &file)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while (!file.atEnd())
    {
        QString rec = file.readLine();
        rec.replace("\n", "");
        QStringList list = rec.split(" ");
        if(!list[0].compare(username) && !list[1].compare(password))
        {
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
