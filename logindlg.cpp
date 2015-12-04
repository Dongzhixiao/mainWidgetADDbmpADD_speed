#include "logindlg.h"
#include "ui_logindlg.h"
#include <QtGui>
#include <QMessageBox>

loginDlg::loginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDlg)
{
    ui->setupUi(this);
    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
}

loginDlg::~loginDlg()
{
    delete ui;
}

void loginDlg::on_loginBtn_clicked()
{
    if(ui->usrLineEdit->text()==tr("qt")&&ui->pwdLineEdit->text()==tr("123456"))
    //�ж��û����������Ƿ���ȷ
    accept();
    else{
    QMessageBox::warning(this,tr("Warning"),tr("user name or password error!"),QMessageBox::Yes);
    //�������ȷ����������Ի���
    }

}
