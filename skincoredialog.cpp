#include "skincoredialog.h"
#include "ui_skincoredialog.h"

skinCoreDialog::skinCoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::skinCoreDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("ֱ�����"));

    QRegExp rx("^((180|\\d?\\d|1[1-7]\\d)(\\.\\d{1,2})?)$");   //������ʽ:0~180,������λС��
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->lineEdit_degree->setValidator(pReg);

    QRegExp rx2("^(5|[1-4](\\.\\d{1,2})?|0\\.[1-9]\\d?|0\\.0[1-9])$");  //������ʽ:0~180,������λС��
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->lineEdit_infillSpace->setValidator(pReg2);

}

skinCoreDialog::~skinCoreDialog()
{
    delete ui;
}

void skinCoreDialog::outlineData(std::vector<xd::outlines> a)
{
    theData.clear();
    theData=a;
}


void skinCoreDialog::on_pushButton_confirm_clicked()
{

}
