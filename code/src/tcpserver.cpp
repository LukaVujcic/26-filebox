#include "tcpserver.h"

TCPRunnable *TCPServer::createRunnable()
{
    qDebug() << this << "Creating runnable...";

    TCPRunnable *runnable = new TCPRunnable();
    runnable->setAutoDelete(false);

    return runnable;
}

void TCPServer::incomingConnection(qintptr handle)
{
    int count = connections();

    if(maxConnections != 0 && count >= maxConnections)
    {
        qDebug() << this << "************************ Rejecting connection:" << handle;
        qDebug() << this << " Count: " << count << " Max:" << maxConnections;

        reject(handle);

        return;
    }

    switch (threadMode)
    {
        case MODE_SINGLE:
            acceptSingle(handle);
            break;
        case MODE_POOLED:
            acceptPooled(handle);
            break;
        case MODE_THREADED:
            acceptThreaded(handle);
            break;
        default:
            break;
    }
}

void TCPServer::startRunnable(TCPRunnable *runnable)
{
    if(!runnable)
    {
        qWarning() << this << "Runnable is null!";
        return;
    }

    qDebug() << this << "Starting " << runnable;

    runnable->setAutoDelete(false);

    runnables.append(runnable);

    qDebug() << this << "Connecting signal and slots";

    connect(this, &TCPServer::closing, runnable, &TCPRunnable::closing, Qt::QueuedConnection);
    connect(runnable, &TCPRunnable::started, this, &TCPServer::started, Qt::QueuedConnection);
    connect(runnable, &TCPRunnable::finished, this, &TCPServer::finished, Qt::QueuedConnection);
    connect(this, &TCPServer::connecting, runnable, &TCPRunnable::connecting, Qt::QueuedConnection);
    connect(this, &TCPServer::idle, runnable, &TCPRunnable::idle, Qt::QueuedConnection);

    qDebug() << this << "created " << runnable;

    if(threadMode == MODE_THREADED)
    {
        runnable->setThreadedMode(true);
    }

    QThreadPool::globalInstance()->start(runnable);
}

void TCPServer::reject(qintptr handle)
{
    qDebug() << this << "************************ Rejecting connection: " << handle;

    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(handle);
    socket->close();
    socket->deleteLater();
}

void TCPServer::acceptSingle(qintptr handle)
{   
    qDebug() << this << "Accepting in single mode:" << handle;

    if(runnables.count() == 0)
    {
        qCritical() << this << "no runnables to accept connection: " << handle;
        return;
    }

    TCPRunnable *runnable = runnables.at(0);

    if(!runnable)
    {
        qCritical() << this << "could not find runnable to accept connection: " << handle;
        return;
    }

    accept(handle,runnable);
}

void TCPServer::acceptPooled(qintptr handle)
{
    qDebug() << this << "Accepting in pooled mode:" << handle;

    int previous = 0;
    TCPRunnable *runnable = runnables.at(0);

    foreach(TCPRunnable *item, runnables)
    {
        int count = item->count();

        if(count == 0 || count < previous)
        {
            runnable = item;
            break;
        }

        previous = count;
    }

    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    accept(handle,runnable);
}

void TCPServer::acceptThreaded(qintptr handle)
{
    qDebug() << this << "Accepting in threaded mode:" << handle;

    TCPRunnable *runnable = createRunnable();
    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    startRunnable(runnable);
    accept(handle,runnable);
}

void TCPServer::startSingle()
{
    qDebug() << this << "MODE_SINGLE - starting one runnable";
    QThreadPool::globalInstance()->setMaxThreadCount(1);

    TCPRunnable *runnable = createRunnable();
    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    startRunnable(runnable);
}

void TCPServer::startPooled()
{
    qDebug() << this << "MODE_POOLED - starting all runnables";

    QThreadPool pool;
    QThreadPool::globalInstance()->setMaxThreadCount(pool.maxThreadCount());

    for(int i = 0; i < QThreadPool::globalInstance()->maxThreadCount(); i++)
    {
        TCPRunnable *runnable = createRunnable();
        if(!runnable)
        {
            qWarning() << this << "Could not find runable!";
            return;
        }

        startRunnable(runnable);
    }
}

void TCPServer::startThreaded()
{
    qDebug() << this << "MODE_THREADED - runnables will be created as connections are accepted";
    QThreadPool::globalInstance()->setMaxThreadCount(maxConnections);
}

int TCPServer::connections()
{
    int count = 0;
    foreach(TCPRunnable *item, runnables)
    {
        int value = item->count();
        count += value;
    }

    return count;
}

void TCPServer::accept(qintptr handle, TCPRunnable *runnable)
{
    qDebug() << "Called in TCP";
    qDebug() << this << "accepting" << handle << "on" << runnable;

    TCPConnection *connection = new TCPConnection;

    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    emit connecting(handle, runnable, connection);
}

