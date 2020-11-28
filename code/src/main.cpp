#include "headers/login.h"
#include "headers/register.h"
#include "headers/filebox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FileBox fileBox;
    Login login(&fileBox);
    Register reg(&login);

    login.setRegister(&reg);
    fileBox.setFormLogin(&login);

    login.show();

    return a.exec();
}
