#include "login.h"
#include "ui_login.h"
#include "tcpclient.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
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


void Login::on_pbRegister_clicked()
{
   hide();
   m_reg->show();
}

void Login::setFormRegister(Register *r)
{
    m_reg = r;
}

void Login::on_pbLogin_clicked()
{
    QString username = ui->leUsername->text();
    QString password = ui->lePassword->text();
    QString IPAddress = ui->leIP->text();


    if(username.size() == 0) return;
    if(password.size() == 0) return;
    //if(IPAddress.size() == 0) return;

    TCPClient socket("127.0.0.1", 5000);

    socket.sendMessage("LOGIN\n");
    socket.sendMessage(username + "\n");
    socket.sendMessage(password);

    socket.waitForReadyRead();
    QString answer = socket.readLine();

    if(!answer.compare("CONTINUE"))
    {
        hide();
        parentWidget()->show();
    }
    else if(!answer.compare("ERROR"))
    {
        ui->lblWarning->setText("Wrong username or password");
    }

    socket.close();
}












