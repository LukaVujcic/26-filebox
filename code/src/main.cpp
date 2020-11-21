#include "headers/login.h"
#include "headers/register.h"
#include "headers/filebox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;
    w.show();
    return a.exec();
}
