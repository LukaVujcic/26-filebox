#include "login.h"
#include "ui_login.h"
#include "tcpclient.h"

#include <filebox.h>
Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    connect(ui->pbLogin,&QPushButton::clicked,this,&Login::pbLogin_clicked);
    connect(ui->pbRegister,&QPushButton::clicked,this,&Login::pbRegister_clicked);
    setWindowFlag(Qt::Window);
}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void Login::pbRegister_clicked()
{
   hide();
   m_reg->show();

   resetForm();
}

void Login::setFormRegister(Register *r)
{
    m_reg = r;
}

void Login::pbLogin_clicked()
{
    QString username = ui->leUsername->text();
    QString password = ui->lePassword->text();
    QString IPAddress = ui->leIP->text();

    if(username.size() == 0) return;
    if(password.size() == 0) return;
    if(IPAddress.size() == 0) return;

    TCPClient *socket = new TCPClient(IPAddress, 5000);

    if(!socket->isValid() || !socket->waitForConnected(1)){
        socket->close();
        ui->lblWarning->setText("Connection not established!");
        return;
    }

    socket->sendMessage("LOGIN\r\n");
    socket->sendMessage(username + "\n");
    socket->sendMessage(password);

    socket->waitForReadyRead();
    QString answer = socket->readLine();

    if(!answer.compare("CONTINUE"))
    {
        qobject_cast<FileBox*>(parentWidget())->setSocket(socket);
        hide();
        parentWidget()->show();

        resetForm();
    }
    else if(!answer.compare("ERROR"))
    {
        ui->lblWarning->setText("Wrong username or password");
        ui->lePassword->setText("");
        return;
    }
}

void Login::resetForm()
{
    ui->leUsername->setText("");
    ui->lePassword->setText("");
    ui->leIP->setText("");
    ui->lblWarning->setText("");
}












