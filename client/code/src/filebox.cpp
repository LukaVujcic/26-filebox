#include "filebox.h"
#include "ui_filebox.h"
#include "tcpclient.h"

#include <QMessageBox>

FileBox::FileBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileBox)
{
   ui->setupUi(this);

   QFileSystemModel *modelLocal = new QFileSystemModel;
   QFileSystemModel *modelRemote = new QFileSystemModel;

   modelLocal->setRootPath("");
   modelRemote->setRootPath("");

   ui->twLocalFiles->setModel(modelLocal);
   ui->twLocalFiles->setRootIndex(modelLocal->index(""));

   ui->twRemoteFiles->setModel(modelRemote);
   ui->twRemoteFiles->setRootIndex(modelRemote->index(""));
}


FileBox::~FileBox()
{
    delete ui->twLocalFiles->model();
    delete ui->twRemoteFiles->model();
    delete ui;
}

void FileBox::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FileBox::setFormLogin(Login *l)
{
    login = l;
}
void FileBox::on_pbUpload_clicked()
{
    TCPClient socket("127.0.0.1", 5000);

    auto [localFolders,localFiles] = ui->twLocalFiles->getSelectedFiles();
    auto [remoteFolders,remoteFiles] = ui->twRemoteFiles->getSelectedFiles();
    if(remoteFolders.size() + remoteFiles.size() > 1)
    {
        QMessageBox::warning(this, "Upload", "Only one folder can be selected!");
    } else if(remoteFiles.size() > 0)
    {
        QMessageBox::warning(this, "Upload", "Folder can be selected!");
    }


    /*socket.sendMessage("UPLOAD\r\n");
    socket.sendMessage("C:\\Users\\Petar\\Desktop\\testSlanje.png\r\n");
    socket.sendFile("C:\\Users\\Petar\\Desktop\\warning_1_filebox.png");

    socket.waitForReadyRead(-1);
    qDebug() << socket.readLine(1000);*/


    //socket.sendAll(localFiles,localFolders);
    socket.close();
}
