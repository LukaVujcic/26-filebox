#include "register.h"
#include "tcpclient.h"
#include "ui_register.h"

#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>

Register::Register(QWidget *parent) : QWidget(parent), ui(new Ui::Register)
{
      ui->setupUi(this);
      connect(ui->pbRegister, &QPushButton::clicked, this, &Register::pbRegister_clicked);
      setWindowFlag(Qt::Window);
}

Register::~Register() { delete ui; }

void Register::paintEvent(QPaintEvent *)
{
      QStyleOption opt;
      opt.init(this);
      QPainter p(this);
      style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Register::pbRegister_clicked()
{
      QString username = ui->leUsername->text();
      QString password = ui->lePassword->text();
      QString confirmPassword = ui->leConfirmPassword->text();
      QString IPAddress = ui->leIP->text();

      if (!checkInput(username, password, confirmPassword, IPAddress))
      {
            return;
      }

      TCPClient socket(IPAddress, 5000);

      if (!socket.isValid() || !socket.waitForConnected(1000))
      {
            socket.close();
            ui->lblWarning->setText("Connection not established!");
            return;
      }

      socket.sendMessage("REGISTER\r\n");
      socket.sendMessage(username + "\n");
      socket.sendMessage(password);

      socket.waitForReadyRead();
      QString answer = socket.readLine();

      if (!answer.compare("EXISTS"))
      {
            ui->lblWarning->setText("Username is already used");
      }
      else if (!answer.compare("CONTINUE"))
      {

            resetForm();


            QMessageBox::information(this, "Register", "Registration was successful");

            hide();
            parentWidget()->show();
      }
      else if (!answer.compare("ERROR"))
      {
            ui->lblWarning->setText("Can't use that password");
      }

      socket.close();
}

bool Register::checkInput(QString &username, QString &password, QString &confirmPassword, QString &IPAddress)
{
      if (username.size() == 0 || password.size() == 0 || confirmPassword.size() == 0 || IPAddress.size() == 0)
      {
            ui->lblWarning->setText("All lines need to be filled");
            return false;
      }

      if (password.compare(confirmPassword))
      {
            ui->lblWarning->setText("Passwords must match");
            return false;
      }

      if (username.contains(" ") || password.contains(" "))
      {
            ui->lblWarning->setText("Can't use blank space");
            return false;
      }

      return true;
}

void Register::resetForm()
{
    ui->lblWarning->setText("");
    ui->leUsername->setText("");
    ui->lePassword->setText("");
    ui->leConfirmPassword->setText("");
    ui->leIP->setText("");
}
