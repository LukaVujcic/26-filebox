#include "register.h"
#include "ui_register.h"
#include "tcpclient.h"

#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>

Register::Register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
}

Register::~Register()
{
    delete ui;
}

void Register::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void Register::on_pbRegister_clicked()
{
    QString username = ui->leUsername->text();
    QString password = ui->lePassword->text();
    QString confirmPassword = ui->leConfirmPassword->text();

    if(!checkInput(username, password, confirmPassword))
    {
        return;
    }

    TCPClient socket("127.0.0.1", 5000);

    socket.sendMessage("REGISTER\n");
    socket.sendMessage(username + "\n");
    socket.sendMessage(password);

    socket.waitForReadyRead();
    QString answer = socket.readLine();

    ui->lblWarning->setText(answer);

    if(!answer.compare("EXISTS"))
    {
        ui->lblWarning->setText("Username is already used");
    } else if(!answer.compare("CONTINUE"))
    {
        ui->lblWarning->setText("");
        ui->leUsername->setText("");
        ui->lePassword->setText("");
        ui->leConfirmPassword->setText("");

        hide();
        parentWidget()->show();
    }

    /*if(username.size() == 0) return;
    if(password.size() == 0) return;
    if(confirmPassword.size() == 0) return;
    if(password != confirmPassword) return;*/

    socket.close();
}

bool Register::checkInput(QString &username, QString &password, QString &confirmPassword)
{
    if(username.size() == 0 || password.size() == 0 || confirmPassword.size() == 0)
    {
        ui->lblWarning->setText("All lines need to be filled");
        return false;
    }

    if(password.compare(confirmPassword))
    {
        ui->lblWarning->setText("Passwords must match");
        return false;
    }



    return true;
}
