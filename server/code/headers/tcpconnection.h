#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTime>
#include <QFile>
#include <QDir>
#include <QString>

#include <filesystem>
#include <zipper/unzipper.h>
#include <zipper/zipper.h>
#include <vector>
#include <climits>
#include <string>
using namespace zipper;

namespace fs = std::filesystem;

class TCPConnection : public QObject
{
    Q_OBJECT
public:
    explicit TCPConnection(QObject *parent = nullptr);
    ~TCPConnection();
    int idleTime();

    void sendFile(QString filePath);

signals:
    void opened();
    void closed();

public slots:
    virtual void quit();
    virtual void accept(qintptr descriptor);
    virtual void connected();
    virtual void disconnected();
    virtual void readyRead();
    virtual void bytesWritten(qint64 bytes);
    virtual void stateChanged(QAbstractSocket::SocketState socketState);
    virtual void error(QAbstractSocket::SocketError socketError);

private:
    bool is_upload_request(QByteArray& msg);
    bool is_new_folder_request(QByteArray& msg);
    bool is_cut_request(QByteArray& msg);
    bool is_copy_request(QByteArray& msg);
    bool is_paste_request(QByteArray& msg);
    bool is_delete_request(QByteArray& msg);
    bool is_register_request(QByteArray& msg);
    bool is_login_request(QByteArray& msg);
    bool is_rename_request(QByteArray &msg);
    bool is_clear_request(QByteArray &msg);

    bool checkUsername(const QString& username, QFile &file);
    bool checkProfile(const QString& username, const QString& password, QFile &file);
    bool is_filesystem_request(QByteArray& msg);
    bool is_download_request(QByteArray& msg);
    /*bool transfer(fs::path folder_path);

    bool make_folder(fs::path folder_path);
    bool cut();
    bool copy(fs::path folder_path);
    bool paste(fs::path folder_path);*/

private:
    std::vector<fs::path> selected_files;
    bool cut_clicked {false};
    bool copy_clicked {false};

protected:
    QList<QTcpSocket*> sockets;
    QTime activity;

    QTcpSocket* createSocket();
    void active();
};

#endif // TCPCONNECTION_H
