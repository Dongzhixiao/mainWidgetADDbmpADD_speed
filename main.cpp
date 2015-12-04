//王晓东 2015/4/18
//今天得到的经验：1. qt编程slot和signal必须有Q_OBJECT宏，否则就算定义槽函数也不会识别！！如果继承的类不能有这个宏且要使用slot或signal，可在
//父类中写一个虚slot或signal，然后子类重写即可。  2.窗口的子部件之间slot和signal不能传递，因为一传递打开子部件就会死机！这时可以采用子部件和
//窗口连接，窗口再传递给另一个子部件的方式进行间接连接，这样就不会死机了！原因不明啊！！！？？？？

#include <QApplication>
#include "mainwindow.h"
#include "logindlg.h"
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QTextCodec::setCodecForTr(QTextCodec::codecForLocale());  以前是显示中文，现在不行了！
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
