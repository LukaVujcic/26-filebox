#include "tcpserver.h"
#include <clientworker.h>

TCPServer::TCPServer(QObject *parent) : QTcpServer(parent) {
    qDebug() << this << "created";
}

TCPServer::~TCPServer()
{
    qDebug()<<"Server is down, long live the Server!";
}

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    ClientWorker *thread = new ClientWorker(socketDescriptor, this);
    connect(thread, &ClientWorker::finished, thread, &ClientWorker::deleteLater);
    connect(thread, &ClientWorker::task_done, this, &TCPServer::on_task_done);
    thread->start();
}

void TCPServer::on_task_done(QString message)
{
    qDebug()<<"Nit je zavrsila sa radom!"<<"\n";
}


