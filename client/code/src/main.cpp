#include "login.h"
#include "register.h"
#include "filebox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FileBox fileBox;
    Login login(&fileBox);
    Register reg(&login);

    login.setFormRegister(&reg);
    fileBox.setFormLogin(&login);

    login.show();

    return a.exec();
}
