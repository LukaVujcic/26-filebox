#include <QCoreApplication>
#include "tcpconnection.h"
#include "tcprunnable.h"
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPServer server;
    server.listen(QHostAddress::Any, 5000);

    return a.exec();
}
