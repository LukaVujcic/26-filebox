#include "filebox.h"
#include "tcpclient.h"
#include "ui_filebox.h"
#include <QMessageBox>

FileBox::FileBox(QWidget *parent) : QWidget(parent), ui(new Ui::FileBox)
{
      ui->setupUi(this);
      connect(ui->pbUpload, &QPushButton::clicked, this, &FileBox::pbUpload_clicked);
      connect(ui->pbCut, &QPushButton::clicked, this, &FileBox::pbCut_clicked);
      connect(ui->pbCopy, &QPushButton::clicked, this, &FileBox::pbCopy_clicked);
      connect(ui->pbDelete, &QPushButton::clicked, this, &FileBox::pbDelete_clicked);
      connect(ui->pbNewFolder, &QPushButton::clicked, this, &FileBox::pbNewFolder_clicked);
      connect(ui->pbDownload, &QPushButton::clicked, this, &FileBox::pbDownload_clicked);
      connect(ui->pbPaste, &QPushButton::clicked, this, &FileBox::pbPaste_clicked);
      connect(ui->pbRename, &QPushButton::clicked, this, &FileBox::pbRename_clicked);


     // connect(m_socket, &TCPClient::moveOperationsFinished, this, &FileBox::moveOperationsFinished);
      ui->twLocalFiles->setViewFolder("");
      ui->twRemoteFiles->setViewFolder("");

      ui->twRemoteFiles->hideColumn(1);
      ui->twRemoteFiles->setColumnWidth(0, 200);
      ui->twLocalFiles->setColumnWidth(0, 200);
}

FileBox::~FileBox()
{
      delete ui->twLocalFiles->model();
      delete ui->twRemoteFiles->model();

      delete m_socket;
      delete ui;
}

void FileBox::paintEvent(QPaintEvent *)
{
      QStyleOption option;
      option.init(this);
      QPainter painter(this);
      style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

void FileBox::setFormLogin(Login *login) { m_login = login; }

void FileBox::setSocket(TCPClient *socket)
{
      m_socket = socket;
      transferThread = new QThread();
      m_socket->moveToThread(transferThread);
      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
      connect(m_socket, &TCPClient::multiSelectFinished, this, &FileBox::multiSelectFinished);
      connect(m_socket, &TCPClient::uploadFinished, this, &FileBox::uploadFinished);
      connect(m_socket, &TCPClient::downloadFinished, this, &FileBox::downloadFinished);
      connect(m_socket, &TCPClient::newFolderFinished, this, &FileBox::newFolderFinished);
      connect(m_socket, &TCPClient::renameFinished, this, &FileBox::renameFinished);
      connect(m_socket, &TCPClient::pasteFinished, this, &FileBox::pasteFinished);
}

void FileBox::setUserFolder(QString username)
{
    m_userFolder = QDir("filesystem-" + username + "/filesystem").absolutePath();
    m_username = username;
}

void FileBox::changeEnableButtons(bool enable)
{
    QList<QPushButton *> buttons = findChildren<QPushButton *>();
    for (const auto&button:buttons)
    {
        button->setEnabled(enable);
    }
}

void FileBox::pbUpload_clicked()
{


      // TCPClient socket("127.0.0.1", 5000);
      auto [localFolders, localFiles] = ui->twLocalFiles->getSelectedFiles();
      auto [remoteFolders, remoteFiles] = ui->twRemoteFiles->getSelectedFiles();

      if (remoteFolders.size() + remoteFiles.size() > 1)
      {
            QMessageBox::warning(this, "Upload", "Only one folder can be selected!");
            return;
      }
      else if (remoteFiles.size() > 0)
      {
            QMessageBox::warning(this, "Upload", "Folder can be selected!");
            return;
      }
      if (localFolders.size() + localFiles.size() == 0)
      {
            QMessageBox::warning(this, "Upload", "Nothing selected for upload!");
            return;
      }
      //changeEnabledButtons(false);
      if (remoteFolders.size() == 1)
      {
            auto rootPath =
                dynamic_cast<QFileSystemModel *>(ui->twRemoteFiles->model())->rootDirectory().absolutePath();
            auto serverPath = remoteFolders[0].right(remoteFolders[0].length() - rootPath.length());
             transferThread= QThread::create(std::bind(&TCPClient::sendAll,m_socket,localFiles,localFolders,serverPath));
      }
      if (remoteFolders.size() == 0)
      {
            transferThread= QThread::create(std::bind(&TCPClient::sendAll,m_socket,localFiles,localFolders,""));
      }

      changeEnableButtons(false);
      QMessageBox::information(this,"Upload","Upload started!");
      connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
      transferThread->start();
      //ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
}

void FileBox::pbNewFolder_clicked()
{
      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if (files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
      {
            QString destination = (folders.size() == 1) ? folders[0] : "";
            //destination = destination.right(destination.size() - m_userFolder.size());
            transferThread = QThread::create(std::bind(&TCPClient::folderRequest, m_socket, destination, m_userFolder));

            changeEnableButtons(false);

            ui->pbNewFolder->setStyleSheet("background: #800000; color: #333;");
            connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
            transferThread->start();
      }
      else
      {
            qDebug() << "Please, select just one folder!";
      }
}

void FileBox::pbCut_clicked()
{

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();
      transferThread=QThread::create(std::bind(&TCPClient::multiSelect,m_socket,files,folders,"CUT\r\n",m_userFolder));
      changeEnableButtons(false);

      ui->pbCut->setStyleSheet("background: #800000; color: #333;");
      connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
      transferThread->start();
}

void FileBox::pbCopy_clicked()
{
    //connect(m_socket, &TCPClient::moveOperationsFinished, this, &FileBox::moveOperationsFinished);
    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();
    transferThread=QThread::create(std::bind(&TCPClient::multiSelect,m_socket,files,folders,"COPY\r\n",m_userFolder));
    changeEnableButtons(false);

    ui->pbCopy->setStyleSheet("background: #800000; color: #333;");
    connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
    transferThread->start();
}

void FileBox::pbPaste_clicked()
{
      //m_socket->sendMessage("PASTE\r\n");

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if (files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
      {
            QString destination = (folders.size() == 1) ?
                                  folders[0].right(folders[0].size() - m_userFolder.size()) + "\r\n" :
                                  "\r\n";

            qDebug() << destination;

            transferThread=QThread::create(std::bind(&TCPClient::pasteRequest,m_socket,destination));
            changeEnableButtons(false);

            ui->pbPaste->setStyleSheet("background: #800000; color: #333;");
            connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
            transferThread->start();
            //m_socket->sendMessage(destination);
      }
      else
      {
            QMessageBox::warning(this, "Paste", "Please, select just one folder!");
      }

      /*m_socket->waitForReadyRead(-1);
      qDebug() << m_socket->readLine(1000);

      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);*/
}

void FileBox::pbDelete_clicked()
{
      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if(folders.size() + files.size() > 0)
      {
          transferThread=QThread::create(std::bind(&TCPClient::multiSelect,m_socket,files,folders,"DELETE\r\n",m_userFolder));
          changeEnableButtons(false);

          ui->pbDelete->setStyleSheet("background: #800000; color: #333;");
          connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
          transferThread->start();
      }
      else
      {
          QMessageBox::warning(this, "Delete", "Please, select at least one file!");
      }
}
void FileBox::pbRename_clicked()
{
      QString newName = ui->txtEdit->toPlainText();

      if(newName.size() == 0){
          QMessageBox::warning(this, "Rename", "Please, type new name!");
          return;
      }

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if (folders.size() == 1 && files.isEmpty())
      {
            qDebug() << folders[0];
            transferThread=QThread::create(std::bind(&TCPClient::renameRequest,m_socket,folders[0],m_userFolder,newName));
      }
      else if (files.size() == 1 && folders.isEmpty())
      {
            qDebug() << files[0];
            transferThread=QThread::create(std::bind(&TCPClient::renameRequest,m_socket,files[0],m_userFolder,newName));
      }
      else
      {
            // qDebug() << "Lose selektovanje";
            QMessageBox::warning(this, "Rename", "Please, select just one file!");
            return;
      }

      changeEnableButtons(false);

      ui->pbRename->setStyleSheet("background: #800000; color: #333;");
      connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
      transferThread->start();
}
void FileBox::pbDownload_clicked()
{
      auto [remoteFolders, remoteFiles] = ui->twRemoteFiles->getSelectedFiles();
      auto [localFolders, localFiles] = ui->twLocalFiles->getSelectedFiles();
      if (localFiles.size() > 0)
      {
            QMessageBox::warning(this, "Download", "File is selected! Only folder can be selected!");
            return;
      }
      if (localFolders.size() > 1)
      {
            QMessageBox::warning(this, "Download", "Multiple folders selected! Please select at most one folder!");
            return;
      }
      auto rootPath = dynamic_cast<QFileSystemModel *>(ui->twRemoteFiles->model())->rootDirectory().absolutePath();
      transferThread= QThread::create(std::bind(&TCPClient::downloadRequest,m_socket,remoteFiles,remoteFolders,localFolders[0],rootPath));

      changeEnableButtons(false);
      QMessageBox::information(this,"Download","Download started!");
      connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
      transferThread->start();
}
void FileBox::uploadFinished()
{
    QMessageBox::information(this,"Upload","Upload is completed!");
    ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
    changeEnableButtons(true);
}
void FileBox::downloadFinished()
{
    QMessageBox::information(this,"Download","Download is completed!");
    changeEnableButtons(true);
}
void FileBox::multiSelectFinished(const QString& operation)
{
    //QMessageBox::information(this,operation,operation+" is completed!");
    if(!operation.compare("CUT"))
    {
        ui->pbCut->setStyleSheet("");
    }
    else if(!operation.compare("COPY"))
    {
        ui->pbCopy->setStyleSheet("");
    }
    else if(!operation.compare("DELETE"))
    {
        ui->pbDelete->setStyleSheet("");
        ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
    }

    changeEnableButtons(true);
}

void FileBox::newFolderFinished()
{
    ui->pbNewFolder->setStyleSheet("");
    ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
    changeEnableButtons(true);
}

void FileBox::renameFinished()
{
    ui->pbRename->setStyleSheet("");
    ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
    changeEnableButtons(true);
}

void FileBox::pasteFinished()
{
    ui->pbPaste->setStyleSheet("");
    ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
    changeEnableButtons(true);
}
