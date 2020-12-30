#ifndef LOGIN_H
#define LOGIN_H

#include <QPainter>
#include <QStyleOption>
#include <QWidget>

#include "register.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
      class Login;
}
QT_END_NAMESPACE

class Login : public QWidget
{
      Q_OBJECT

     public:
      Login(QWidget *parent = nullptr);
      ~Login() override;

      void paintEvent(QPaintEvent *) override;

      void setFormRegister(Register *r);

     private slots:
      void pbRegister_clicked();

      void pbLogin_clicked();

     private:
      Ui::Login *ui;

      Register *m_register;

      void resetForm();
};
#endif  // LOGIN_H
