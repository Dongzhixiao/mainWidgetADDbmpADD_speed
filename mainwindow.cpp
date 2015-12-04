#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <glwidget.h>
#include <QtGui>
#include "qands.h"
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include "generate_line.h"
#include "displaylayerdialog.h"
#include "bmpdialog.h"
#include "skincoredialog.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    glwidget * displayF=new glwidget(this);  //建立用于显示的窗口
    this->setCentralWidget(displayF);
    OutlineData.clear();
    connect(this,SIGNAL(passData(std::vector<xd::outlines>)),displayF,SLOT(setDrawData(std::vector<xd::outlines>)));
    connect(this,SIGNAL(passLayer(int, int)),displayF,SLOT(setShowLayer(int,int)));
    connect(this,SIGNAL(passInfillData(std::vector<xd::outlines>)),displayF,SLOT(setInfillDrawData(std::vector<xd::outlines>)));
}



void MainWindow::on_action_O_triggered()
{
	
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开一个slc文件"),tr("/请选择slc文件"),tr("切片文件(*.slc)"));
    if(fileName.length()==0)
        return;
    OutlineData.clear();
    InfillData.clear();
    Layer=0;
    std::string cFileName=q2s(fileName);
    std::ifstream readslc;
    /*
    char fileNameT[100];
    strcpy(fileNameT,cFileName);
    */
    readslc.open(cFileName.c_str(),std::ios::in | std::ios::binary);
        if(!readslc )
        {
            cout << "无法打开文件\n";
            cin.get();
            exit(1);
        }
        char m[3]={};
        while(!((char(0x0d)==m[0])&(char(0x0a)==m[1])&(char(0x1a)==m[2])))
        {
            readslc.read(m,3);
            readslc.seekg(-2,ios::cur);
        }
        //auto a=readslc.tellg();
        readslc.seekg(2,ios::cur);  //走过开头部分。
        readslc.seekg(256,ios::cur); //走过文件的预留部分。
        readslc.seekg(1+4*4,ios::cur); //走过条目部分。
        //轮廓数据开始读取！
        char EndSign[9]={0};
        readslc.read(EndSign,8);
        readslc.seekg(-8,ios::cur);

        OutlineData.clear();

        vector<float> recordZ;

        while (!((EndSign[4]==char(0xFF))&(EndSign[5]==char(0xFF))&(EndSign[6]==char(0xFF))&(EndSign[7]==char(0xFF))))
        {
            float LayerZ;
            int Num;
            readslc.read((char*)&LayerZ,4);  //这一步相当有意思！
            readslc.read((char*)&Num,4);
            vector<vector<xd::xdpoint> > aa;
            for (int i=1;i<=Num;i++)
            {
                vector<xd::xdpoint> bb;
                int Num2;
                readslc.read((char*)&Num2,4);
                int GapNumber;
                readslc.read((char*)&GapNumber,4);
                for(int j=1;j<=Num2;j++)
                {
                    xd::xdpoint cc;
                    readslc.read((char*)&cc.x,4);
                    readslc.read((char*)&cc.y,4);
                    cc.z=LayerZ;
                    bb.push_back(cc);
                }
                aa.push_back(bb);
            }
            OutlineData.push_back(aa);
            readslc.read(EndSign,8);
            readslc.seekg(-8,ios::cur);
            recordZ.push_back(LayerZ);
            //cout<<"读取了一层"<<endl<<"当前文件指针位置为: "<<readslc.tellg()<<endl;
        }
        //cout<<"文件读取完毕！"<<endl;
        //cout<<"一共有"<<OutlineData.size()<<"层！"<<endl;

        //前处理轮廓线，因为有些轮廓线在x轴上一次出现好多个点，将影响后续判断，将同一条平行线上只保留一个点。
        for (int k=0;k!=OutlineData.size();++k)
        {
            for(int i=0;i!=OutlineData[k].size();i++)
           {
               int j=0;
               while(!xd::IsDealFinish(OutlineData[k][i],j))
               {

                   OutlineData[k][i].erase(OutlineData[k][i].begin()+j);
               }
           }
        }
        //因为删除可能会把最后一个点或者第一个点删掉一个，为了满足一样，必须再加上！
        for (int k=0;k!=OutlineData.size();++k)
        {
            for (int i=0;i!=OutlineData[k].size();i++)
            {
                if ((OutlineData[k][i][0].x!=OutlineData[k][i][OutlineData[k][i].size()-1].x)||(OutlineData[k][i][0].y!=OutlineData[k][i][OutlineData[k][i].size()-1].y))
                {
                    OutlineData[k][i].push_back(OutlineData[k][i][0]);
                }
            }
        }		
        emit passData(OutlineData);
        Layer=OutlineData.size();
}

MainWindow::~MainWindow()
{
    //delete displayF;

    delete ui;

}

void MainWindow::on_action_triggered()
{
    if(OutlineData.empty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请先读入slc文件"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    lineScanDialog *LSD=new lineScanDialog;

    connect(LSD,SIGNAL(sendInfilledData(std::vector<xd::outlines>)),this,SLOT(getInfilledData(std::vector<xd::outlines>)));
    connect(this,SIGNAL(passDataToLSD(std::vector<xd::outlines>)),LSD,SLOT(outlineData(std::vector<xd::outlines>)));

    emit passDataToLSD(OutlineData);
    //connect(LSD,SIGNAL(sendInfilledData(std::vector<xd::outlines>)),displayF,SLOT(setDrawData(std::vector<xd::outlines>)));
    LSD->show();
}

void MainWindow::getInfilledData(std::vector<xd::outlines> a)
{
    InfillData=a;
    emit passInfillData(InfillData);  //发送填充后数据给opengl模块
}

void MainWindow::on_action_2_triggered()
{
    if(OutlineData.empty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请先读入slc文件"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    displayLayerDialog *SL=new displayLayerDialog;
    connect(this,SIGNAL(sendLayer(int)),SL,SLOT(getLayer(int)));
    connect(SL,SIGNAL(sendLN(int,int)),this,SLOT(get_show_layer(int,int)));
    emit(sendLayer(Layer));
    SL->show();
}

void MainWindow::get_show_layer(int aa, int bb)
{
    emit passLayer(aa,bb);
}

void MainWindow::on_action_B_triggered()
{
    if(OutlineData.empty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请先读入slc文件"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    bmpDialog *BMPD=new bmpDialog;

   // connect(BMPD,SIGNAL(sendInfilledData(std::vector<xd::outlines>)),this,SLOT(getInfilledData(std::vector<xd::outlines>)));
    connect(this,SIGNAL(passDataToBMPD(std::vector<xd::outlines>)),BMPD,SLOT(outlineData(std::vector<xd::outlines>)));

    emit passDataToBMPD(OutlineData);
    //connect(LSD,SIGNAL(sendInfilledData(std::vector<xd::outlines>)),displayF,SLOT(setDrawData(std::vector<xd::outlines>)));
    BMPD->exec();
}

void MainWindow::on_action_S_triggered()
{
    if(OutlineData.empty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请先读入slc文件"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    skinCoreDialog *SCD=new skinCoreDialog;
   /*
    connect(SCD,SIGNAL(sendInfilledData(std::vector<xd::outlines>)),this,SLOT(getInfilledData(std::vector<xd::outlines>)));
    connect(this,SIGNAL(passDataToLSD(std::vector<xd::outlines>)),SCD,SLOT(outlineData(std::vector<xd::outlines>)));
    emit passDataToLSD(OutlineData);
    */
    SCD->show();
}
