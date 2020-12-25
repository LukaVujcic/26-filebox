#ifndef FILEBOX_H
#define FILEBOX_H

#include "filesystemtreeview.h"
#include "login.h"

#include <QWidget>
#include <QPainter>
#include <QStyleOption>
#include <tcpclient.h>
QT_BEGIN_NAMESPACE
namespace Ui { class FileBox; }
QT_END_NAMESPACE

class FileBox : public QWidget
{
    Q_OBJECT

public:
    FileBox(QWidget *parent = nullptr);
    ~FileBox();
    void paintEvent(QPaintEvent*);

    void setFormLogin(Login *l);
    void setSocket(TCPClient* socket);

private:
    Ui::FileBox *ui;
    TCPClient *m_socket;
    Login *login;
private slots:
    void on_pbUpload_clicked();

    void on_pbNewFolder_clicked();
    void on_pbCut_clicked();
    void on_pbCopy_clicked();
    void on_pbPaste_clicked();
    void on_pbDelete_clicked();
    void on_pbDownload_clicked();
};
#endif // FILEBOX_H
