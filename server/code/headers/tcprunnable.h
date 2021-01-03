#ifndef TCPRUNNABLE_H
#define TCPRUNNABLE_H

#include "tcpconnection.h"

#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QRunnable>
#include <QThread>

class TCPRunnable : public QThread{
    Q_OBJECT
  public:
    explicit TCPRunnable(QObject *parent = nullptr);

};

#endif // TCPRUNNABLE_H
