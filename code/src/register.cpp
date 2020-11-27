#include "headers/register.h"
#include "ui_register.h"

#include <QStyleOption>
#include <QPainter>

Register::Register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
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

    if(username.size() == 0) return;
    if(password.size() == 0) return;
    if(confirmPassword.size() == 0) return;
    if(password != confirmPassword) return;


    hide();
    parentWidget()->show();
}
