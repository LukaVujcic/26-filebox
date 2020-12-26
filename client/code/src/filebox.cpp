#include "ui_filebox.h"
#include "filebox.h"
#include "tcpclient.h"

#include <QMessageBox>

FileBox::FileBox(QWidget *parent):
    QWidget(parent), ui(new Ui::FileBox)
{
    ui->setupUi(this);
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

void FileBox::paintEvent(QPaintEvent*)
{
    QStyleOption option;
    option.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

void FileBox::setFormLogin(Login *log)
{
    login = log;
}

void FileBox::setSocket(TCPClient *socket)
{
    m_socket = socket;
}

void FileBox::on_pbUpload_clicked()
{
    //TCPClient socket("127.0.0.1", 5000);

    auto [localFolders, localFiles] = ui->twLocalFiles->getSelectedFiles();
    auto [remoteFolders, remoteFiles] = ui->twRemoteFiles->getSelectedFiles();

    if(remoteFolders.size() + remoteFiles.size() > 1)
    {
        QMessageBox::warning(this, "Upload", "Only one folder can be selected!");
    }
    else if(remoteFiles.size() > 0)
    {
        QMessageBox::warning(this, "Upload", "Folder can be selected!");
    }


    /*socket.sendMessage("UPLOAD\r\n");
    socket.sendMessage("C:\\Users\\Petar\\Desktop\\testSlanje.png\r\n");
    socket.sendFile("C:\\Users\\Petar\\Desktop\\warning_1_filebox.png");

    socket.waitForReadyRead(-1);
    qDebug() << socket.readLine(1000);*/

    ui->twRemoteFiles->getServerFilesystem(m_socket);
    //socket.sendAll(localFiles,localFolders);
    //socket.close();
}

void FileBox::on_pbNewFolder_clicked()
{   
    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    m_socket->sendMessage("NEW FOLDER\r\n");
    qDebug() << "Creating new folder...";

    if(files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
    {
        QString destination = (folders.size() == 1) ? folders[0] : "";

        qDebug() << destination;

        m_socket->sendMessage(destination);
    }
    else
    {
        qDebug() << "Please, select just one folder!";
    }
}

void FileBox::on_pbCut_clicked()
{
    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    m_socket->sendMessage("CLEAR\r\n");

    m_socket->waitForReadyRead(-1);
    qDebug() << m_socket->readLine(1000);

    for(const auto &folder: folders)
    {
        m_socket->sendMessage("CUT\r\n");
        qDebug() << folder;
        m_socket->sendMessage(folder);

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }

    for(const auto &file: files)
    {
        m_socket->sendMessage("CUT\r\n");
        qDebug() << file;
        m_socket->sendMessage(file);

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }
}

void FileBox::on_pbCopy_clicked()
{
    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    m_socket->sendMessage("CLEAR\r\n");

    m_socket->waitForReadyRead(-1);
    qDebug() << m_socket->readLine(1000);

    for(const auto &folder: folders)
    {
        m_socket->sendMessage("COPY\r\n");
        qDebug() << folder;
        m_socket->sendMessage(folder + "\r\n");

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }

    for(const auto &file: files)
    {
        m_socket->sendMessage("COPY\r\n");
        qDebug() << file;
        m_socket->sendMessage(file + "\r\n");

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }
}

void FileBox::on_pbPaste_clicked()
{
    m_socket->sendMessage("PASTE\r\n");

    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    if(files.isEmpty() && (folders.size() == 1 || folders.isEmpty()))
    {
        QString destination = (folders.size() == 1) ? folders[0] : "";

        qDebug() << destination;

        m_socket->sendMessage(destination);
    }
    else
    {
        qDebug() << "Please, select just one folder!";
    }
}

void FileBox::on_pbDelete_clicked()
{


    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    for(const auto &folder: folders)
    {
        m_socket->sendMessage("DELETE\r\n");
        qDebug() << folder;
        m_socket->sendMessage(folder + "\r\n");

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }

    for(const auto &file: files)
    {
        m_socket->sendMessage("DELETE\r\n");
        qDebug() << file;
        m_socket->sendMessage(file + "\r\n");

        m_socket->waitForReadyRead(-1);
        qDebug() << m_socket->readLine(1000);
    }
}
void FileBox::on_pbRename_clicked()
{
    m_socket->sendMessage("RENAME\r\n");

    auto [folders, files] = ui->twRemoteFiles->getSelectedFiles();

    if(folders.size() == 1 && files.isEmpty())
    {
        qDebug() << folders[0];
        m_socket->sendMessage(folders[0] + "\r\n");
    }
    else if(files.size() == 1 && folders.isEmpty())
    {
        qDebug() << files[0];
        m_socket->sendMessage(files[0] + "\r\n");
    }
    else
    {
        qDebug() << "Lose selektovanje";
    }

    m_socket->sendMessage(ui->txtEdit->toPlainText().trimmed() + "\r\n");
}


void FileBox::on_pbDownload_clicked(){
    //qDebug()<<QDateTime::currentMSecsSinceEpoch();
    auto [remoteFolders, remoteFiles] = ui->twRemoteFiles->getSelectedFiles();
    auto [localFolders, localFiles] = ui->twLocalFiles->getSelectedFiles();
    auto rootPath=dynamic_cast<QFileSystemModel*>(ui->twRemoteFiles->model())->rootDirectory().absolutePath();
    m_socket->downloadRequest(remoteFiles,remoteFolders,localFolders[0],rootPath);

//    QVector<QString> selected;
//    selected=remoteFolders;
//    for (const auto& file:remoteFiles){
//        selected.push_back(file);
//    }
//    for (const auto& selectItem: selected)
//    {
//        //qDebug()<<folder;
//        //qDebug()<<dynamic_cast<QFileSystemModel*>(ui->twRemoteFiles->model())->rootDirectory().absolutePath();
//        auto rootPath=dynamic_cast<QFileSystemModel*>(ui->twRemoteFiles->model())->rootDirectory().absolutePath();
//        auto itemPath=selectItem.right(selectItem.length()-rootPath.length());
//        qDebug()<<itemPath;
//        m_socket->downloadRequest(itemPath);
//        QFileInfo qfi(selectItem);
//        m_socket->receiveFile(qfi.fileName());
//    }




}

