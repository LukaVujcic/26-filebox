#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QThread>
#include <QString>
#include <QTcpSocket>
#include <QMutex>

class ClientWorker : public QThread
{
    Q_OBJECT
public:
    ClientWorker(qintptr socket_descriptor, QObject* parent = nullptr);

protected:
    void run() override;

signals:
    void task_done(QString status_message);

private:
    void upload(QString userFolder);

private:
  static QMutex* mutex;
  static QMap<QTcpSocket*, QString> users_map;

private:
    int _socket_descriptor;
    QString _task;
    QString _users_folder;
    QTcpSocket* _socket;

    static const QString USERS_FOLDER;
    static const QString USERS_LOGIN_INFO;
    static const char* ZIPPER_LOCATION;
};

#endif // CLIENTWORKER_H
