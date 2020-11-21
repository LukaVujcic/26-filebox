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

