#include "displaylayerdialog.h"
#include "ui_displaylayerdialog.h"

displayLayerDialog::displayLayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::displayLayerDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle(tr("��ʾ"));
    ui->spinBox->setMinimum(1);
    ui->spinBox_2->setMinimum(1);
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(lowChange(int)));
    connect(ui->spinBox_2,SIGNAL(valueChanged(int)),this,SLOT(highChange(int)));
}

displayLayerDialog::~displayLayerDialog()
{
    delete ui;
}

void displayLayerDialog::getLayer(int a)
{
    Layer=a;
    ui->label->adjustSize();
    ui->label->setWordWrap(true);
    ui->label->setAlignment(Qt::AlignTop);
    ui->label->setGeometry(QRect(5, 5, 160, 27*4));  //�ı��о�
    QString qian=tr("һ����");
            qian+=QString::number(Layer, 10);;
            qian+=tr("�㣬������Ҫ��ʾ��Ŀ�ʼ�����ͽ�������:");
            ui->label->setText(qian);
            ui->spinBox->setMaximum(Layer);
            ui->spinBox_2->setMaximum(Layer);
            ui->spinBox_2->setValue(Layer);
            //ui->spinBox->set
}

void displayLayerDialog::lowChange(int lowN)
{
    if(lowN>ui->spinBox_2->value())
        ui->spinBox_2->setValue(lowN);
    emit sendLN(ui->spinBox->value(),ui->spinBox_2->value());
}

void displayLayerDialog::highChange(int highN)
{
    if(highN<ui->spinBox->value())
        ui->spinBox->setValue(highN);
     emit sendLN(ui->spinBox->value(),ui->spinBox_2->value());
}
