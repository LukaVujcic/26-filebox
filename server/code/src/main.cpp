#include "tcpconnection.h"
#include "tcprunnable.h"
#include "tcpserver.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    TCPServer server;
    qDebug()<<"Tata nit: "<<QThread::currentThread()<<"\n";
    server.listen(QHostAddress::Any, 5000);

    return a.exec();
}
