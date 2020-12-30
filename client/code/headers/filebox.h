#ifndef FILEBOX_H
#define FILEBOX_H

#include "filesystemtreeview.h"
#include "login.h"

#include <tcpclient.h>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
      class FileBox;
}
QT_END_NAMESPACE

class FileBox : public QWidget
{
      Q_OBJECT

     public:
      FileBox(QWidget *parent = nullptr);
      ~FileBox() override;
      void paintEvent(QPaintEvent *) override;

      void setFormLogin(Login *l);
      void setSocket(TCPClient *socket);
      void setUserFolder(QString username);

     private:
      Ui::FileBox *ui;
      TCPClient *m_socket;
      Login *m_login;
      QString m_userFolder;
      QString m_username;
     private slots:
      void pbUpload_clicked();

      void pbNewFolder_clicked();
      void pbCut_clicked();
      void pbCopy_clicked();
      void pbPaste_clicked();
      void pbDelete_clicked();
      void pbRename_clicked();
      void pbDownload_clicked();
};
#endif  // FILEBOX_H
