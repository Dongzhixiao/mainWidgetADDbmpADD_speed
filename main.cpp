//������ 2015/4/18
//����õ��ľ��飺1. qt���slot��signal������Q_OBJECT�꣬������㶨��ۺ���Ҳ����ʶ�𣡣�����̳е��಻�����������Ҫʹ��slot��signal������
//������дһ����slot��signal��Ȼ��������д���ɡ�  2.���ڵ��Ӳ���֮��slot��signal���ܴ��ݣ���Ϊһ���ݴ��Ӳ����ͻ���������ʱ���Բ����Ӳ�����
//�������ӣ������ٴ��ݸ���һ���Ӳ����ķ�ʽ���м�����ӣ������Ͳ��������ˣ�ԭ��������������������

#include <QApplication>
#include "mainwindow.h"
#include "logindlg.h"
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QTextCodec::setCodecForTr(QTextCodec::codecForLocale());  ��ǰ����ʾ���ģ����ڲ����ˣ�
//    Widget w;
//    loginDlg login;
//        if(login.exec()==QDialog::Accepted)
//        {
//            w.show();
//            return a.exec();
//        }
//        else return 0;

     MainWindow w;
     w.show();     
     a.exec();

}
