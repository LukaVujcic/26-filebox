#ifndef FILEBOX_H
#define FILEBOX_H

#include <QWidget>
#include <QPainter>
#include <QStyleOption>
#include "filesystemtreeview.h"
#include "login.h"

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

private:
    Ui::FileBox *ui;

    Login *login;
private slots:
    void on_pbUpload_clicked();

};
#endif // FILEBOX_H
