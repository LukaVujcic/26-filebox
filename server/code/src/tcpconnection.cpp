#include "tcpconnection.h"
#include <QDir>
#include <QFileInfo>
#include <fstream>
#include <clientworker.h>

namespace fs = std::filesystem;

bool find_str(char const* str, char const* substr);

TCPConnection::TCPConnection(QObject* parent) : QObject(parent)
{
      Q_UNUSED(parent);
      qDebug() << this << "created";
}

TCPConnection::~TCPConnection() { qDebug() << this << "destroyed"; }

int TCPConnection::idleTime() { return activity.secsTo(QTime::currentTime()); }

bool TCPConnection::is_download_request(QByteArray& msg) { return !QString(msg).compare(("DOWNLOAD\r\n")); }

void TCPConnection::on_task_done(QString status)
{
    if(!status.compare("OK"))
    {
        qDebug()<<"Tasked is completed!";
        return;
    }

    qDebug()<<"Task failed successfully"<<"\n";
}

bool TCPConnection::is_filesystem_request(QByteArray& msg) { return !QString(msg).compare(("FILESYSTEM\r\n")); }

bool TCPConnection::is_upload_request(QByteArray& msg) { return !QString(msg).compare(("UPLOAD\r\n")); }

bool TCPConnection::is_new_folder_request(QByteArray& msg) { return !QString(msg).compare(("NEW FOLDER\r\n")); }

bool TCPConnection::is_cut_request(QByteArray& msg) { return !QString(msg).compare(("CUT\r\n")); }

bool TCPConnection::is_copy_request(QByteArray& msg) { return !QString(msg).compare(("COPY\r\n")); }

bool TCPConnection::is_paste_request(QByteArray& msg) { return !QString(msg).compare(("PASTE\r\n")); }

bool TCPConnection::is_delete_request(QByteArray& msg) { return !QString(msg).compare(("DELETE\r\n")); }

bool TCPConnection::is_register_request(QByteArray& msg) { return !QString(msg).compare(("REGISTER\r\n")); }

bool TCPConnection::is_login_request(QByteArray& msg) { return !QString(msg).compare(("LOGIN\r\n")); }

bool TCPConnection::is_rename_request(QByteArray& msg) { return !QString(msg).compare(("RENAME\r\n")); }

bool TCPConnection::is_clear_request(QByteArray& msg) { return !QString(msg).compare(("CLEAR\r\n")); }

void display_files_in_folder(QString current_path, QString original_path, Zipper& zip, QString user)
{
      QDir dir(current_path);

      if (dir.isEmpty())
      {
            char* dir_path = (dir.canonicalPath().right((dir.canonicalPath().size() - original_path.size())) + "/")
                                 .toLocal8Bit()
                                 .data();
            dir_path = (user + static_cast<QString>(dir_path)).toLocal8Bit().data();
            zip.add(dir_path, zip.SaveHierarchy);
            return;
      }

      foreach (auto name, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst))
      {
            if (name.isDir())
            {
                  display_files_in_folder(name.filePath(), original_path, zip, user);
            }
            else
            {
                  char* file_path =
                      name.filePath().right(name.filePath().size() - original_path.size()).toLocal8Bit().data();
                  file_path = (user + static_cast<QString>(file_path)).toLocal8Bit().data();
                  zip.add(file_path, zip.SaveHierarchy);
            }
      }
}

void get_files_and_folders(QString current_path, QString& server_path, Zipper& zip)
{
      QFileInfo file_info(current_path);

      if (file_info.isFile())
      {
            std::ifstream input1(file_info.filePath().toLocal8Bit().data());
            char* file_path = server_path.toLocal8Bit().data();
            zip.add(input1, file_path, zip.SaveHierarchy);
            // free(file_path);

            return;
      }

      QDir dir(file_info.filePath());

      if (dir.isEmpty())
      {
            char* dir_path = (server_path + "/").toLocal8Bit().data();
            zip.add(dir_path, zip.SaveHierarchy);
            // free(dir_path);

            return;
      }

      foreach (auto name, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst))
      {
            QString new_path = server_path + "/" + name.baseName();
            get_files_and_folders(name.filePath(), new_path, zip);
      }
}


void TCPConnection::upload_(QTcpSocket *socket, QString userFolder){
    socket->waitForReadyRead(1000);
    QByteArray file_path = socket->readLine(1000);

    qDebug() << file_path;

    //qDebug() << "Request: " << REQUEST << " file path: " << file_path << "\n";
    qDebug() << "File content: "
             << "\n";
    QFile f((userFolder + QString(file_path)).trimmed());

    // QFile f(QString("C:/Users/bozam/Desktop/preko_mreze.txt").trimmed());

    /*if(f.exists())
            {
                qDebug()<<"File Exists";
                return;
            }*/

    f.open(QIODevice::WriteOnly | QIODevice::Truncate);

    const int chunckSize = 1024 * 1024 * 10;
    char* chunk = new char[chunckSize + 1];
    int total = 0;
    int bytesRead;
    while (true)
    {
        qDebug() << socket->ConnectedState;
        socket->waitForReadyRead(1000);

        if (socket->bytesAvailable() > 0)
            bytesRead = socket->read(chunk, chunckSize);
        else
        {
            qDebug() << "bytesRead: " << bytesRead;
            qDebug() << "ELSE!!!" << socket->ConnectedState;
            break;
        }

        if (bytesRead >= 0)
        {
            total += bytesRead;
        }
        // qDebug()<<bytesRead;
        if (bytesRead <= 0)
        {
            qDebug() << "bytesRead: " << bytesRead;
            break;
        }
        f.write(static_cast<const char*>(chunk), bytesRead);
        //f.flush();
        f.waitForBytesWritten(1000);
        // f.flush()
    }

    // qDebug()<<"Total: "<<total;
    // qDebug()<<"Nesto";
    // qDebug()<<total<<"BYTES";
    qDebug() << socket->ConnectedState;
    delete[] chunk;
    f.close();
    qDebug() << "Pisanje se zavrsava"
             << "\n";
    //        qDebug()<<socket->ConnectedState;
    qDebug() << socket->write("OK\r\n");
    //socket->flush();
    socket->waitForBytesWritten(1000);
}

bool TCPConnection::checkProfile(const QString& username, const QString& password, QFile& file)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while (!file.atEnd())
    {
        QString line = file.readLine();
        line.replace("\n", "");

        QStringList list = line.split(" ");
        if (list.size() < 2)
        {
            return false;
        }
        QString matchPassword = list[1];

        if (list.size() > 2)
        {
            for (int i = 2; i < list.size(); i++)
            {
                matchPassword = matchPassword + " " + list[i];
            }
        }

        if (!list[0].compare(username) && !matchPassword.compare(password))
        {
            return true;
        }
    }

    return false;
}

void TCPConnection::login_(QTcpSocket* socket, QString USERS_LOGIN_INFO, QMap<QTcpSocket*, QString> users_map, QString& username)
{
    socket->waitForReadyRead(100);
    username = socket->readLine();
    socket->waitForReadyRead(100);
    QByteArray password = socket->readLine();

    username.replace("\n", "");

    // pamtimo korisnika u mapi
    users_map[socket] = username;


    QFile file(USERS_LOGIN_INFO);

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password);

    qDebug() << username << password;

    password = hash.result();
    hash.reset();

    if (TCPConnection::checkProfile(username, password, file))
    {
        qDebug()<<"Korisnik postoji, login je moguc!";
        socket->write("CONTINUE");
        //socket->flush(); // TODO: ovoga nije bilo!
        socket->waitForBytesWritten(100);
    }
    else
    {
        socket->write("ERROR");
        //socket->flush(); // TODO: Ovoga nije bilo!
        socket->waitForBytesWritten(100);
    }

    file.close();

    qDebug()<<"Dosli smo do kraja!"<<"\n";
}

void TCPConnection::copy_(QTcpSocket *socket, QString userFolder)
{
    // selected_files.clear();
    cut_clicked = false;
    copy_clicked = true;

    socket->waitForReadyRead(1000);
    QString file_path = userFolder + socket->readLine(1000);

    qDebug() << file_path;

    selected_files.emplace_back(file_path.trimmed().toStdWString());

    socket->write("OK\r\n");
    //socket->flush();
    socket->waitForBytesWritten();
}

void TCPConnection::paste_(QTcpSocket *socket, QString userFolder)
{
    socket->waitForReadyRead(1000);
    QString destination_path = (userFolder + socket->readLine(1000)).trimmed();

    qDebug() << destination_path << "******";

    try
    {
        if (selected_files.empty())
        {
            qDebug() << "*** Nothing selected! ***";
        }

        for (const auto& source : selected_files)
        {
            const fs::path destination{destination_path.toStdWString()};

            const fs::path file_name{source.filename()};
            const fs::path new_path{destination / file_name};

            const auto copy_options = fs::copy_options::update_existing | fs::copy_options::recursive |
                                      fs::copy_options::copy_symlinks;

            if (fs::is_directory(source))
            {
                fs::create_directory(new_path);
            }

            fs::copy(source, new_path, copy_options);
        }
        if (cut_clicked)
        {
            for (const auto& file : selected_files)
            {
                std::uintmax_t n = fs::remove_all(file);

                if (n == static_cast<uintmax_t>(-1)) qDebug() << "Nothing deleted";
            }

            selected_files.clear();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << e.what();
    }

    socket->write("OK\r\n");
    //socket->flush();
    socket->waitForBytesWritten();
}

void TCPConnection::sendFile(QString filePath, QTcpSocket* socket)
{
      //auto* socket = static_cast<QTcpSocket*>(sender());
      qDebug()<<"Send file!";
      QFile file(filePath);
      // qDebug()<<filePath;
      if (!file.open(QIODevice::ReadOnly)) return;
      const int chunckSize = 1024 * 1024 * 10;
      char* chunk = new char[chunckSize + 1];
      int total = 0;
      qint64 fileSize = file.size();
      // qDebug()<<"fileSize: "<<fileSize;
      auto message = (QString::number(fileSize) + "\r\n");
      // qDebug()<<"Poruka"<<(message+"\r\n").toLocal8Bit().data();
      qDebug()<<"pre-write";
      qDebug()<<socket->write((message + "\r\n").toLocal8Bit().data());
      //qDebug()<<"write";
      //socket->flush();
      socket->waitForBytesWritten(1000);
      while (true)
      {
            int bytesRead = file.read(chunk, chunckSize);
            socket->write(static_cast<const char*>(chunk), bytesRead);
            //socket->flush(); // TODO: ovoga nije bilo!!!
            socket->waitForBytesWritten();
            if (bytesRead == 0)
            {
                  break;
            }
            total += bytesRead;
      }
      delete[] chunk;
      file.close();
}

void TCPConnection::download_(QTcpSocket* socket, QString userFolder, const char* ZIPPER_LOCATION)
{
    bool conversion_successful = true;

    socket->waitForReadyRead(1000);
    int num_of_files = socket->readLine(1000).toInt(&conversion_successful);
    qDebug() << "Broj linija: " << num_of_files << "\n";

    Zipper zipper(ZIPPER_LOCATION);

    if (!conversion_successful)
    {
        qDebug() << "Expected number of lines!"
                 << "\n";
        return;
    }

    QString server_path;

    while (num_of_files--)
    {
        socket->waitForReadyRead(1000);
        QString path = socket->readLine(1000);

        server_path = userFolder + path.trimmed();
        qDebug() << "Cela putanja: " << server_path << "\n";

        QFileInfo f1(server_path);
        QString name = f1.baseName();

        get_files_and_folders(server_path, name, zipper);
    }

    zipper.close();

    sendFile(ZIPPER_LOCATION, socket);
    QFile zip(ZIPPER_LOCATION);
    zip.remove();
}

void TCPConnection::filesystem_(QTcpSocket* socket, QString userFolder, const char* ZIPPER_LOCATION)
{
    qDebug()<<"Krecem filesystem"<<"\n";
    QDir dir(userFolder);
    Zipper zipper(ZIPPER_LOCATION);

    display_files_in_folder(dir.absolutePath(), dir.absolutePath(), zipper, "filesystem");
    zipper.close();
    sendFile(ZIPPER_LOCATION,socket);
    QFile zip(ZIPPER_LOCATION);
    zip.remove();
    qDebug()<<"Zavrsavam filesystem!"<<"\n";
}

void TCPConnection::register_(QTcpSocket* socket, QString users_login_info)
{
    socket->waitForReadyRead(100);
    QString username = socket->readLine();
    socket->waitForReadyRead(100);
    QByteArray password = socket->readLine();

    username.replace("\n", "");

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password);

    qDebug() << username << password;

    password = hash.result();
    hash.reset();

    QFile file(users_login_info);

    if (checkUsername(username, file))
    {
        socket->write("EXISTS");
        //socket->flush(); //TODO: ovoga nije bilo!!!
        socket->waitForBytesWritten();
    }
    else
    {
        socket->write("CONTINUE");
        //socket->flush(); // TODO: ovoga nije bilo!!!
        socket->waitForBytesWritten();
        file.seek(file.size());
        QString line = username + " " + password + "\n";
        file.write(line.toStdString().c_str());
        QDir folder("../users");
        folder.mkdir(username);
    }

    file.close();
}

void TCPConnection::delete_(QTcpSocket *socket, QString userFolder)
{
    qDebug()<<"\n"<<"Pre cekanja!"<<"\n";
    socket->waitForReadyRead(1000);
    qDebug()<<"\nNakon cekanja"<<"\n";
    QString file_path = userFolder + socket->readLine(1000);

    qDebug() << file_path;

    try
    {
        // delete izdvojiti u posebnu metodu koja ce se pozivati ovde i u paste funkciji

        const fs::path file_name{file_path.trimmed().toStdWString()};
        std::uintmax_t n = fs::remove_all(file_name);

        if (n == static_cast<uintmax_t>(-1))
            qDebug() << "Nothing deleted";
        else
            qDebug() << "Deleted " << n << "files or directories";
    }
    catch (const std::exception& e)
    {
        qDebug() << e.what();
    }

    socket->write("OK\r\n");
    qDebug()<<"Pre slanja!"<<"\n";
    //socket->flush();
    socket->waitForBytesWritten();
    qDebug()<<"Nakon slanja!"<<"\n";
    qDebug()<<"********************************************************************************************\n";
}

void TCPConnection::new_folder_(QTcpSocket* socket, QString userFolder)
{
    socket->waitForReadyRead(1000);
    std::string new_folder = (socket->readLine(1000)).trimmed().toStdString();

    socket->waitForReadyRead(1000);
    QString path = (userFolder + socket->readLine(1000)).trimmed();

    qDebug()<<path<<"\n";

    // std::string new_folder {"New folder"};

    // qDebug() << name;
    // std::string new_folder = name.toStdString();

    const fs::path parent_path{path.trimmed().toStdWString()};
    const fs::path folder_path{parent_path / new_folder};

    if (!fs::exists(folder_path))
    {
        fs::create_directory(folder_path);
        qDebug() << "****Folder created succesfully!****";
    }
    else
    {
        // Kako cuvati sledecu slobodnu vrednost, a da ne pocinje uvek od 2?
        // Problem sa magicnim promenljivama...
        unsigned start = 2;

        while (start <= ULONG_MAX)
        {
            std::string tmp_folder{new_folder + " (" + std::to_string(start) + ")"};
            const fs::path new_folder_path{parent_path / tmp_folder};

            if (fs::exists(new_folder_path))
            {
                start++;
                continue;
            }
            else
            {
                fs::create_directory(new_folder_path);
                break;
            }
        }
        // qDebug() << "****Failed creating folder (folder exists)!****";
    }
    qDebug()<<"NEW FOLDER COnnected state before: "<<socket->ConnectedState;
    qDebug() << socket->write("OK\r\n");
    qDebug()<<"NEW FOLDER COnnected state after: "<<socket->ConnectedState;

    //    socket->flush();

    qDebug()<<"NEW FOLDER COnnected wait state before: "<<socket->ConnectedState;
    socket->waitForBytesWritten(10000);
    qDebug()<<"NEW FOLDER COnnected wait state after: "<<socket->ConnectedState;
}

void TCPConnection::rename_(QTcpSocket *socket, QString userFolder)
{
    socket->waitForReadyRead(1000);
    QString file_path = (userFolder + socket->readLine(1000)).trimmed();

    qDebug() << file_path;

    socket->waitForReadyRead(1000);
    QString input_name = (socket->readLine(1000)).trimmed();

    qDebug() << input_name;


    try
    {
        const fs::path old_file_path{file_path.trimmed().toStdString()};
        const fs::path parent_path{old_file_path.parent_path()};

        const fs::path new_file_name{input_name.trimmed().toStdString()};
        const fs::path new_file_path{parent_path / new_file_name};

        rename(old_file_path, new_file_path);
    }
    catch (const std::exception& e)
    {
        qDebug() << e.what();
    }


    socket->write("OK\r\n");
    //socket->flush();
    socket->waitForBytesWritten();
}

void TCPConnection::cut_(QTcpSocket* socket, QString userFolder)
{
    cut_clicked = true;
    copy_clicked = false;

    socket->waitForReadyRead(1000);
    QString file_path = userFolder + socket->readLine(1000);

    qDebug() << file_path;

    selected_files.emplace_back(file_path.trimmed().toStdWString());

    socket->write("OK\r\n");
    //socket->flush();
    socket->waitForBytesWritten();
}

bool TCPConnection::checkUsername(const QString& username, QFile& file)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while (!file.atEnd())
    {
        QString rec = file.readLine();
        rec.replace("\n", "");
        QStringList list = rec.split(" ");
        if (!list[0].compare(username))
        {
            return true;
        }
    }

    return false;
}


