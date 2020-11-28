#include "headers/filebox.h"
#include "ui_filebox.h"

FileBox::FileBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileBox)
{
   ui->setupUi(this);
}

FileBox::~FileBox()
{
    delete ui;
}

void FileBox::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FileBox::setFormLogin(Login *l){
    login = l;
}

