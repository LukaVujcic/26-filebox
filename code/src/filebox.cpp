#include "filebox.h"
#include "ui_filebox.h"

FileBox::FileBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileBox)
{
   ui->setupUi(this);
   QFileSystemModel *model = new QFileSystemModel;
   model->setRootPath("");
   ui->twLocalFiles->setModel(model);
   ui->twLocalFiles->setRootIndex(model->index(""));
}

FileBox::~FileBox()
{
    delete ui->twLocalFiles->model();
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
void FileBox::on_pbUpload_clicked()
{
}
