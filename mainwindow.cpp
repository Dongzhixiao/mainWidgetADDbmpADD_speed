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

    glwidget * displayF=new glwidget(this);  //����������ʾ�Ĵ���
    this->setCentralWidget(displayF);
    OutlineData.clear();
    connect(this,SIGNAL(passData(std::vector<xd::outlines>)),displayF,SLOT(setDrawData(std::vector<xd::outlines>)));
    connect(this,SIGNAL(passLayer(int, int)),displayF,SLOT(setShowLayer(int,int)));
    connect(this,SIGNAL(passInfillData(std::vector<xd::outlines>)),displayF,SLOT(setInfillDrawData(std::vector<xd::outlines>)));
}



void MainWindow::on_action_O_triggered()
{
	
    QString fileName = QFileDialog::getOpenFileName(this,tr("��һ��slc�ļ�"),tr("/��ѡ��slc�ļ�"),tr("��Ƭ�ļ�(*.slc)"));
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
            cout << "�޷����ļ�\n";
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
        readslc.seekg(2,ios::cur);  //�߹���ͷ���֡�
        readslc.seekg(256,ios::cur); //�߹��ļ���Ԥ�����֡�
        readslc.seekg(1+4*4,ios::cur); //�߹���Ŀ���֡�
        //�������ݿ�ʼ��ȡ��
        char EndSign[9]={0};
        readslc.read(EndSign,8);
        readslc.seekg(-8,ios::cur);

        OutlineData.clear();

        vector<float> recordZ;

        while (!((EndSign[4]==char(0xFF))&(EndSign[5]==char(0xFF))&(EndSign[6]==char(0xFF))&(EndSign[7]==char(0xFF))))
        {
            float LayerZ;
            int Num;
            readslc.read((char*)&LayerZ,4);  //��һ���൱����˼��
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
            //cout<<"��ȡ��һ��"<<endl<<"��ǰ�ļ�ָ��λ��Ϊ: "<<readslc.tellg()<<endl;
        }
        //cout<<"�ļ���ȡ��ϣ�"<<endl;
        //cout<<"һ����"<<OutlineData.size()<<"�㣡"<<endl;

        //ǰ���������ߣ���Ϊ��Щ��������x����һ�γ��ֺö���㣬��Ӱ������жϣ���ͬһ��ƽ������ֻ����һ���㡣
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
        //��Ϊɾ�����ܻ�����һ������ߵ�һ����ɾ��һ����Ϊ������һ���������ټ��ϣ�
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
        QMessageBox::warning(this,tr("����"),tr("���ȶ���slc�ļ�"), QMessageBox::Yes, QMessageBox::Yes);
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
    emit passInfillData(InfillData);  //�����������ݸ�openglģ��
}

void MainWindow::on_action_2_triggered()
{
    if(OutlineData.empty())
    {
        QMessageBox::warning(this,tr("����"),tr("���ȶ���slc�ļ�"), QMessageBox::Yes, QMessageBox::Yes);
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
        QMessageBox::warning(this,tr("����"),tr("���ȶ���slc�ļ�"), QMessageBox::Yes, QMessageBox::Yes);
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
        QMessageBox::warning(this,tr("����"),tr("���ȶ���slc�ļ�"), QMessageBox::Yes, QMessageBox::Yes);
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
