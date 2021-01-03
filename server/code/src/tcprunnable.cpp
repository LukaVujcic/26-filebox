#include "tcprunnable.h"

TCPRunnable::TCPRunnable(QObject *parent) {
    Q_UNUSED(parent);
    qDebug() << this << "created";
}
