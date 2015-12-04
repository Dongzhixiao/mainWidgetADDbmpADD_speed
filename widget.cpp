#include "widget.h"
#include "ui_widget.h"
#include <QtGui>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    GLWidget5 * displayF=new GLWidget5(10,this);
    QHBoxLayout *rightlayout=new QHBoxLayout;
    rightlayout->addWidget(displayF);
    setLayout(rightlayout);
}

Widget::~Widget()
{
    delete ui;

}
