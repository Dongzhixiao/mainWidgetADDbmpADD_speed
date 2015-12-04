#include "linescandialog.h"
#include "ui_linescandialog.h"
#include <iostream>
#include <fstream>
#include <QtGui>
#include "qands.h"
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>

lineScanDialog::lineScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lineScanDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("直线填充"));
    theData.clear();
    ui->pushButton_save->setEnabled(FALSE);

    QRegExp rx("^((180|\\d?\\d|1[1-7]\\d)(\\.\\d{1,2})?)$");   //正则表达式:0~180,可以两位小数
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->lineEdit->setValidator(pReg);

    QRegExp rx2("^(5|[1-4](\\.\\d{1,2})?|0\\.[1-9]\\d?|0\\.0[1-9])$");  //正则表达式:0~180,可以两位小数
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->lineEdit_2->setValidator(pReg2);

    QRegExp rx3("^(\\d{1,4})$");   //正则表达式:可以输入4位整数
    QRegExpValidator *pReg3 = new QRegExpValidator(rx3, this);
    ui->lineEdit_infillSpeed->setValidator(pReg3);
    ui->lineEdit_outlineSpeed->setValidator(pReg3);

    ui->label_E->hide();
    ui->lineEdit_E->hide();
    ui->progressBar->hide();
    ui->label_generateInfill->hide();
    ui->label_generateFinish->hide();

    setWindowFlags(Qt::WindowStaysOnTopHint);
}

lineScanDialog::~lineScanDialog()
{
    delete ui;
}

void lineScanDialog::on_pushButton_clicked()
{
    theData.clear();
    this->close();
}


void lineScanDialog::on_pushButton_confirm_clicked()
{
     float degree=ui->lineEdit->text().toFloat();
     float lineWidth=ui->lineEdit_2->text().toFloat();
     int offsetNumber=ui->lineEdit_offsetNumber->text().toInt();
     if(!(degree>=0&&degree<=180&&lineWidth>0&&lineWidth<=5))
     {
         QMessageBox::warning(this,tr("警告"),tr("请输入合法数字：角度[0,180]且线宽(0,5]！"), QMessageBox::Yes, QMessageBox::Yes);
         return;
     }
//     else
//     {
//         std::cout<<degree<<std::endl;
//         std::cout<<lineWidth<<std::endl;
//         return;
//     }
     ui->label_generateFinish->hide();
     ui->progressBar->reset();
     ui->label_generateInfill->show();
     ui->progressBar->setMaximum(theData.size());
     ui->progressBar->setMinimum(1);
     ui->progressBar->show();
     for(int i=0;i!=theData.size();++i)
         recordZ.push_back(theData[i][0][0].z);
    //为了FDM打印，必须加载一层底座！否则很容易堵住喷头，而且零件不容易取下来
    xd::xdpoint limitPositionA(theData[0][0][0].x,theData[0][0][0].y),limitPositionB(theData[0][0][0].x,theData[0][0][0].y);
    for (int i=0;i!=theData[0].size();++i)
    {
        for (int j=1;j!=theData[0][i].size();++j)
        {
            if (limitPositionA.x<theData[0][i][j].x)
            {
                limitPositionA.x=theData[0][i][j].x;
            }
            if (limitPositionA.y<theData[0][i][j].y)
            {
                limitPositionA.y=theData[0][i][j].y;
            }
            if (limitPositionB.x>theData[0][i][j].x)
            {
                limitPositionB.x=theData[0][i][j].x;
            }
            if (limitPositionB.y>theData[0][i][j].y)
            {
                limitPositionB.y=theData[0][i][j].y;
            }
        }
    }
    limitPositionA.x+=4;	limitPositionA.y+=4;   //比最大位置多4
    limitPositionB.x-=4;	limitPositionB.y-=4;   //比最小位置小4
    xd::outline totalProfile;
    xd::outlines totalProfiles;
    firstResult.clear();
    secondResult.clear();
    totalProfile.push_back(xd::xdpoint(limitPositionA.x,limitPositionA.y));
    totalProfile.push_back(xd::xdpoint(limitPositionA.x,limitPositionB.y));
    totalProfile.push_back(xd::xdpoint(limitPositionB.x,limitPositionB.y));
    totalProfile.push_back(xd::xdpoint(limitPositionB.x,limitPositionA.y));
    totalProfile.push_back(xd::xdpoint(limitPositionA.x,limitPositionA.y));  //记得加上初始点
    totalProfiles.push_back(totalProfile);
    xd::InfillLine(totalProfiles,firstResult,1.8,0,0);                    //底层间距1.8
    xd::InfillLine(totalProfiles,secondResult,1.8,90,0);

    //普通直线扫描线生成：
    for (int i=0;i!=theData.size();i++)
    {
        std::vector<xd::outline> aa;
        if (!theData[i].empty())
        {
            if(i%2==0)
            {
                xd::InfillLine(theData[i],aa,lineWidth,degree,offsetNumber);
                //xd::InfillOffset(theData[i],aa,lineWidth);
            }
            else
            {
                xd::InfillLine(theData[i],aa,lineWidth,degree+90,offsetNumber);
                //xd::InfillOffset(theData[i],aa,lineWidth);
            }
        }
        ui->progressBar->setValue(i+1);
        InfilledData.push_back(aa);
    }
    ui->label_generateFinish->show();
    ui->label_generateInfill->hide();
    ui->progressBar->hide();

    emit sendInfilledData(InfilledData);
    ui->pushButton_save->setEnabled(TRUE);
    ui->label_E->show();
    ui->lineEdit_E->show();
}

void lineScanDialog::outlineData(std::vector<xd::outlines> a)
{
    theData.clear();
    theData=a;

}

void lineScanDialog::on_pushButton_save_clicked()
{
    float deltaE=ui->lineEdit_E->text().toFloat();
    float outlineSpeed=ui->lineEdit_outlineSpeed->text().toFloat();
    float infillSpeed=ui->lineEdit_infillSpeed->text().toFloat();
    if(!(deltaE>0&&deltaE<99))
    {
        QMessageBox::warning(this,tr("警告"),tr("请输入合法数字：E的变化量(0,99)！"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

    QString str = time.toString(tr("yyyy年MM月dd日hh时mm分ss秒_ddd")); //设置显示格式

    QString fileName = QFileDialog::getSaveFileName(this,tr("保存文件"),str,tr("FDM文件(*.gcode)"));
    if(fileName.length()==0)
        return;
    //std::string cFileName=q2s(fileName);
    //std::ofstream outfile(cFileName);
    QFile qFile(fileName);
    qFile.open(QIODevice::WriteOnly);
    QTextStream outfile(&qFile);

    outfile. setRealNumberNotation(QTextStream::FixedNotation);
    outfile. setRealNumberPrecision(3);

    /*右喷头
    outfile<<"M103"<<endl<<"M73 P0"<<endl<<"G21"<<endl<<"G90"<<endl<<"M109 S110 T0"<<endl<<"M104 S220 T0"<<endl<<"G162 X Y F2500"<<endl<<"G161 Z F1100"<<endl;
    outfile<<"G92 Z-5"<<endl<<"G1 Z0.0"<<endl<<"G161 Z F100"<<endl<<"M132 X Y Z A B"<<endl<<"G1 X-110.5 Y-74 Z150 F3300.0"<<endl<<"G130 X20 Y20 Z20 A20 B20"<<endl<<"M6 T0"<<endl;
    outfile<<"G130 X127 Y127 Z40 A127 B127"<<endl<<"M108 R3.0 T0"<<endl<<"G0 X-110.5 Y-74"<<endl<<"G0 Z0.6"<<endl<<"M108 R5.0"<<endl<<"M101"<<endl<<"G4 P2000"<<endl;
    */
    //左喷头

    outfile<<"M103"<<endl<<"M73 P0"<<endl<<"G21"<<endl<<"G90"<<endl<<"M109 S110 T1"<<endl<<"M104 S220 T1"<<endl<<"G162 X Y F2500"<<endl<<"G161 Z F1100"<<endl;
    outfile<<"G92 Z-5"<<endl<<"G1 Z0.0"<<endl<<"G161 Z F100"<<endl<<"M132 X Y Z A B"<<endl<<"G1 X-110.5 Y-74 Z150 F3300.0"<<endl<<"G130 X20 Y20 Z20 A20 B20"<<endl<<"M6 T1"<<endl;
    outfile<<"G130 X127 Y127 Z40 A127 B127"<<endl<<"M108 R3.0 T1"<<endl<<"G0 X-110.5 Y-74"<<endl<<"G0 Z0.6"<<endl<<"M108 R5.0"<<endl<<"M101"<<endl<<"G4 P2000"<<endl;

    float ShangCiE=1;
    float XiShuE=deltaE;  //E的比例系数取得这个值相当于普通丝径设置为1.82mm时的数值。
    float X=0,Y=0;

    float deltaZ=0.27;
    if (1)    //这里的代码表示两层互相成90度夹角的底层，为了打完零件后容易取下来！！
    {
        deltaZ+=0.2;
        //第一层E系数是0.189，层厚是0.47,速度F值为372
        X=firstResult[0][0].x;
        Y=firstResult[0][0].y;
        outfile<<"G1 "<<"X"<<X<<" Y"<<Y<<" Z"<<deltaZ<<" F3300"<<endl;
        outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE<<endl<<"G1 F3300"<<endl<<"M101"<<endl;
        for (int j=1;j!=firstResult[0].size();++j)
        {
            ShangCiE+=sqrt(pow((X-firstResult[0][j].x),2)+pow((Y-firstResult[0][j].y),2))*0.189;
            outfile<<"G1 "<<"X"<<firstResult[0][j].x<<" Y"<<firstResult[0][j].y<<" Z"<<deltaZ<<" F372"<<" E"<<ShangCiE<<endl;
            X=firstResult[0][j].x;
            Y=firstResult[0][j].y;
        }
        outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE-1<<endl<<"G1 F1230"<<endl<<"M103"<<endl;
        //第二层E系数是0.0739，层厚是0.98,速度F值为1111
        deltaZ+=0.51;
        X=secondResult[0][0].x;
        Y=secondResult[0][0].y;
        outfile<<"G1 "<<"X"<<X<<" Y"<<Y<<" Z"<<deltaZ<<" F3360"<<endl;
        outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE<<endl<<"G1 F3360"<<endl<<"M101"<<endl;
        for (int j=1;j!=secondResult[0].size();++j)
        {
            ShangCiE+=sqrt(pow((X-secondResult[0][j].x),2)+pow((Y-secondResult[0][j].y),2))*0.0739;
            outfile<<"G1 "<<"X"<<secondResult[0][j].x<<" Y"<<secondResult[0][j].y<<" Z"<<deltaZ<<" F1111"<<" E"<<ShangCiE<<endl;
            X=secondResult[0][j].x;
            Y=secondResult[0][j].y;
        }
        outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE-1<<endl<<"G1 F1230"<<endl<<"M103"<<endl;
        //第三层同第二层目的绝对是为了工件容易从底座分离！！
        deltaZ+=0.51;
        X=secondResult[0][0].x;
        Y=secondResult[0][0].y;
        outfile<<"G1 "<<"X"<<X<<" Y"<<Y<<" Z"<<deltaZ<<" F3360"<<endl;
        outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE<<endl<<"G1 F3360"<<endl<<"M101"<<endl;
        for (int j=1;j!=secondResult[0].size();++j)
        {
            ShangCiE+=sqrt(pow((X-secondResult[0][j].x),2)+pow((Y-secondResult[0][j].y),2))*0.0739;
            outfile<<"G1 "<<"X"<<secondResult[0][j].x<<" Y"<<secondResult[0][j].y<<" Z"<<deltaZ<<" F1111"<<" E"<<ShangCiE<<endl;
            X=secondResult[0][j].x;
            Y=secondResult[0][j].y;
        }
    }
    outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE-1<<endl<<"G1 F1230"<<endl<<"M103"<<endl;
    deltaZ+=0.23;  //这一层并非0.27，可能是为了填补底层的一些空白！

    for (int i=0;i!=theData.size();i++)
    {
        float tZ=recordZ[i]+deltaZ;
        for (int j=0;j!=theData[i].size();j++)   //填充轮廓的循环
        {
            X=theData[i][j][0].x;
            Y=theData[i][j][0].y;
            outfile<<"G1 "<<"X"<<X<<" Y"<<Y<<" Z"<<tZ<<" F3360"<<endl;
            outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE<<endl<<"G1 F3360"<<endl<<"M101"<<endl;
            for (int k=1;k!=theData[i][j].size();k++)
            {
                ShangCiE+=sqrt(pow((X-theData[i][j][k].x),2)+pow((Y-theData[i][j][k].y),2))*XiShuE;
                outfile<<"G1 "<<"X"<<theData[i][j][k].x<<" Y"<<theData[i][j][k].y<<" Z"<<tZ<<" E"<<ShangCiE<<" F"<<outlineSpeed<<endl;
                X=theData[i][j][k].x;
                Y=theData[i][j][k].y;
            }
            outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE-1<<endl<<"G1 F1230"<<endl<<"M103"<<endl;
        }
        for (int j=0;j!=InfilledData[i].size();j++)  //填充内部扫描线的循环
        {
            X=InfilledData[i][j][0].x;
            Y=InfilledData[i][j][0].y;
            outfile<<"G1 "<<"X"<<X<<" Y"<<Y<<" Z"<<tZ<<" F3360"<<endl;
            outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE<<endl<<"G1 F3360"<<endl<<"M101"<<endl;
            for (int k=1;k!=InfilledData[i][j].size();k++)
            {
                ShangCiE+=sqrt(pow((X-InfilledData[i][j][k].x),2)+pow((Y-InfilledData[i][j][k].y),2))*XiShuE;
                outfile<<"G1 "<<"X"<<InfilledData[i][j][k].x<<" Y"<<InfilledData[i][j][k].y<<" Z"<<tZ<<" E"<<ShangCiE<<" F"<<infillSpeed<<endl;
                X=InfilledData[i][j][k].x;
                Y=InfilledData[i][j][k].y;
            }
            outfile<<"G1 F1200"<<endl<<"G1 E"<<ShangCiE-1<<endl<<"G1 F1230"<<endl<<"M103"<<endl;
        }
    }
    //结尾代码
    outfile<<"M73 P100"<<endl<<"G0 Z150"<<endl<<"M18"<<endl<<"M104 S0 T0"<<endl<<"M109 S0 T0"<<endl<<"G162 X Y F2500"<<endl<<"M18"<<endl<<"M70 P5"<<endl<<"M72 P1"<<endl;

    std::cout<<"测试完毕！"<<endl;



    this->close();
}
