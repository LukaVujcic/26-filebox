#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
      class Register;
}
QT_END_NAMESPACE

class Register : public QWidget
{
      Q_OBJECT

     public:
      Register(QWidget *parent = nullptr);
      ~Register();
      void paintEvent(QPaintEvent *);

     private slots:
      void pbRegister_clicked();

     private:
      Ui::Register *ui;

      bool checkInput(QString &username, QString &password, QString &confirmPassword, QString &IPAddress);
};
#endif  // REGISTER_H
