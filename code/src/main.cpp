#include "headers/login.h"
#include "headers/register.h"
#include "headers/filebox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login l;
    Register r(&l);
    r.setWindowFlag(Qt::Window);
    l.setRegister(&r);
    l.show();

    return a.exec();
}
