#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <zipper/unzipper.h>
#include <zipper/zipper.h>
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTime>
#include <climits>
#include <filesystem>
#include <string>
#include <vector>
#include <QMutex>
#include <QMap>
using namespace zipper;

namespace fs = std::filesystem;

class TCPConnection : public QObject
{
    Q_OBJECT

public:
    void upload_(QTcpSocket* socket,QString userFolder);
    void download_(QTcpSocket* socket, QString userFolder, const char* ZIPPER_LOCATION);
    void filesystem_(QTcpSocket* socket, QString userFolder, const char* ZIPPER_LOCATION);

    void register_(QTcpSocket* socket, QString users_login_info);
    void delete_(QTcpSocket* socket, QString userFolder);
    void new_folder_(QTcpSocket* socket, QString userFolder);
    void rename_(QTcpSocket* socket, QString userFolder);
    void cut_(QTcpSocket* socket, QString userFolder);
    void login_(QTcpSocket* socket, QString USERS_LOGIN_INFO, QMap<QTcpSocket*, QString> users_map, QString& login);
    void copy_(QTcpSocket* socket, QString userFolder);
    void paste_(QTcpSocket* socket, QString userFolder);

  const QString USERS_FOLDER = "../users/";
  const QString USERS_LOGIN_INFO = "../users/users.txt";
  const char* ZIPPER_LOCATION = "../users/ziptest.zip";


   public:
    explicit TCPConnection(QObject* parent = nullptr);
    ~TCPConnection();
    int idleTime();

      void sendFile(QString filePath, QTcpSocket* socket);

   signals:
    void opened();
    void closed();


//    virtual void bytesWritten(qint64 bytes);
//    virtual void stateChanged(QAbstractSocket::SocketState socketState);
//    virtual void error(QAbstractSocket::SocketError socketError);

   public:
    void readyRead(QTcpSocket *);
    bool is_upload_request(QByteArray& msg);
    bool is_new_folder_request(QByteArray& msg);
    bool is_cut_request(QByteArray& msg);
      bool is_copy_request(QByteArray& msg);
      bool is_paste_request(QByteArray& msg);
      bool is_delete_request(QByteArray& msg);
      bool is_register_request(QByteArray& msg);
      bool is_login_request(QByteArray& msg);
      bool is_rename_request(QByteArray& msg);
      bool is_clear_request(QByteArray& msg);

      bool checkUsername(const QString& username, QFile& file);
      bool checkProfile(const QString& username, const QString& password, QFile& file);
      bool is_filesystem_request(QByteArray& msg);
      bool is_download_request(QByteArray& msg);
    /*bool transfer(fs::path folder_path);

    bool make_folder(fs::path folder_path);
    bool cut();
    bool copy(fs::path folder_path);
    bool paste(fs::path folder_path);*/


  // Da li mi treba ista ispod ovoga vise uopste?
  private slots:
    void on_task_done(QString status);

   private:
    std::vector<fs::path> selected_files;
    bool cut_clicked{false};
    bool copy_clicked{false};

   protected:
    QList<QTcpSocket*> sockets;
    QTime activity;

    QTcpSocket* createSocket();
    void active();
};

#endif  // TCPCONNECTION_H
