#include "login.h"
#include "ui_login.h"

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
    if(IPAddress.size() == 0) return;

    hide();
    parentWidget()->show();
}












