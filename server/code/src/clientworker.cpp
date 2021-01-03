#include "clientworker.h"
#include <QThread>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>
#include <zipper/unzipper.h>
#include <zipper/zipper.h>
#include <tcpconnection.h>

using namespace zipper;

QMap<QTcpSocket*, QString> ClientWorker::users_map = QMap<QTcpSocket*, QString>();
QMutex* ClientWorker::mutex = new QMutex();

ClientWorker::ClientWorker(qintptr socket_descriptor, QObject* parent)
    :QThread(parent),
    _socket_descriptor(socket_descriptor)
{

}

void ClientWorker::run()
{
    QString USERS_FOLDER = "../users/";
    QString USERS_LOGIN_INFO = "../users/users.txt";
    const char* ZIPPER_LOCATION = "../users/ziptest.zip";
    QString username = "";
    TCPConnection conn;


    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(_socket_descriptor);


    qDebug()<<"Pisem iz niti"<<QThread::currentThread()<<"\n";

    while(true)
    {
        qDebug()<<"Cekam ready read ovde...";
        //socket->flush();
        socket->waitForReadyRead(-1);

        if (socket->state() != socket->ConnectedState)
        {
            break;
        }

        QByteArray REQUEST = socket->readLine(1000);

        qDebug()<<"\nZahtev: "<<REQUEST<<"\n";

        if (conn.is_login_request(REQUEST))
        {
            conn.login_(socket, USERS_LOGIN_INFO, users_map, username);
            ClientWorker::users_map[socket] = username;
            USERS_FOLDER = USERS_FOLDER.append(username);
        }

        else if(conn.is_filesystem_request(REQUEST))
        {
            conn.filesystem_(socket, USERS_FOLDER, ZIPPER_LOCATION);
        }

        else if(conn.is_upload_request(REQUEST))
        {
            conn.upload_(socket, USERS_FOLDER);
        }

        else if(conn.is_download_request(REQUEST))
        {
            conn.download_(socket, USERS_FOLDER, ZIPPER_LOCATION);
        }

        else if(conn.is_register_request(REQUEST))
        {
            conn.register_(socket, USERS_LOGIN_INFO);
        }

        else if(conn.is_new_folder_request(REQUEST))
        {
            qDebug()<<"Connected state: "<< _socket->ConnectedState<<"\n";
            conn.new_folder_(socket, USERS_FOLDER);
        }

        else if(conn.is_delete_request(REQUEST))
        {
             conn.delete_(socket, USERS_FOLDER);
        }

        else if(conn.is_rename_request(REQUEST))
        {
            conn.rename_(socket, USERS_FOLDER);
        }

        else if(conn.is_paste_request(REQUEST))
        {
            conn.paste_(socket, USERS_FOLDER);
        }

        else if(conn.is_copy_request(REQUEST))
        {
            conn.copy_(socket, USERS_FOLDER);
        }

        else if(conn.is_cut_request(REQUEST))
        {
            conn.cut_(socket, USERS_FOLDER);
        }
        else if (conn.is_clear_request(REQUEST)) {
            conn.clear_(socket);

        }

    }

    qDebug()<<"Izlazim iz niti"<<QThread::currentThread()<<"\n";
    emit task_done("DONE!");
}

