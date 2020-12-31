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
      m_socket->moveToThread(transferThread);
      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
      connect(m_socket, &TCPClient::moveOperationsFinished, this, &FileBox::moveOperationsFinished);
      connect(m_socket, &TCPClient::uploadFinished, this, &FileBox::uploadFinished);
      connect(m_socket, &TCPClient::downloadFinished, this, &FileBox::downloadFinished);
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

      m_socket->sendMessage("NEW FOLDER\r\n");
      qDebug() << "Creating new folder...";

      m_socket->sendMessage("New folder\r\n");

      if (files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
      {
            QString destination = (folders.size() == 1) ? folders[0] : "";

            qDebug() << destination;

            m_socket->sendMessage(destination.right(destination.size() - m_userFolder.size()) + "\r\n");
            m_socket->waitForReadyRead(-1);
            qDebug() << m_socket->readLine(1000);
      }
      else
      {
            qDebug() << "Please, select just one folder!";
      }

      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
}

void FileBox::pbCut_clicked()
{

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();
      transferThread=QThread::create(std::bind(&TCPClient::moveOperations,m_socket,files,folders,"CUT\r\n",m_userFolder));
      changeEnableButtons(false);
      QMessageBox::information(this,"Cut","Cut started!");
      connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
      transferThread->start();

      //m_socket->moveOperations(files,folders,"CUT\r\n",m_userFolder);
}

void FileBox::pbCopy_clicked()
{
    //connect(m_socket, &TCPClient::moveOperationsFinished, this, &FileBox::moveOperationsFinished);
    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();
    transferThread=QThread::create(std::bind(&TCPClient::moveOperations,m_socket,files,folders,"COPY\r\n",m_userFolder));
    changeEnableButtons(false);
    QMessageBox::information(this,"Copy","Copy started!");
    connect(transferThread,&QThread::finished,transferThread,&QThread::deleteLater);
    transferThread->start();
}

void FileBox::pbPaste_clicked()
{
      m_socket->sendMessage("PASTE\r\n");

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if (files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
      {
            QString destination = (folders.size() == 1) ?
                                  folders[0].right(folders[0].size() - m_userFolder.size()) + "\r\n" :
                                  "\r\n";

            qDebug() << destination;

            m_socket->sendMessage(destination);
      }
      else
      {
            QMessageBox::warning(this, "Paste", "Please, select just one folder!");
      }

      m_socket->waitForReadyRead(-1);
      qDebug() << m_socket->readLine(1000);

      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
}

void FileBox::pbDelete_clicked()
{
      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      for (const auto &folder : qAsConst(folders))
      {
            m_socket->sendMessage("DELETE\r\n");
            qDebug() << folder;
            m_socket->sendMessage(folder.right(folder.size() - m_userFolder.size()) + "\r\n");

            m_socket->waitForReadyRead(-1);
            qDebug() << m_socket->readLine(1000);
      }

      for (const auto &file : qAsConst(files))
      {
            m_socket->sendMessage("DELETE\r\n");
            qDebug() << file;
            m_socket->sendMessage(file.right(file.size() - m_userFolder.size()) + "\r\n");

            m_socket->waitForReadyRead(-1);
            qDebug() << m_socket->readLine(1000);
      }

      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
}
void FileBox::pbRename_clicked()
{
      if(ui->txtEdit->toPlainText().size() == 0){
          QMessageBox::warning(this, "Rename", "Please, type new name!");
          return;
      }

      m_socket->sendMessage("RENAME\r\n");

      auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

      if (folders.size() == 1 && files.isEmpty())
      {
            qDebug() << folders[0];
            m_socket->sendMessage(folders[0].right(folders[0].size() - m_userFolder.size()) + "\r\n");
      }
      else if (files.size() == 1 && folders.isEmpty())
      {
            qDebug() << files[0];
            m_socket->sendMessage(files[0].right(files[0].size() - m_userFolder.size()) + "\r\n");
      }
      else
      {
            // qDebug() << "Lose selektovanje";
            QMessageBox::warning(this, "Rename", "Please, select just one file!");
            m_socket->sendMessage("ERROR\r\n");
            return;
      }

      m_socket->sendMessage(ui->txtEdit->toPlainText().trimmed() + "\r\n");

      m_socket->waitForReadyRead(-1);
      qDebug() << m_socket->readLine(1000);
      ui->twRemoteFiles->getServerFilesystem(m_socket, m_username);
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
void FileBox::moveOperationsFinished(const QString& operation)
{
    QMessageBox::information(this,operation,operation+" is completed!");
    changeEnableButtons(true);
}
