#include "bmpdialog.h"
#include "ui_bmpdialog.h"
#include "QMessageBox"
#include "QTime"
#include "QFileDialog"
#include <vector>
#include "qands.h"
#include <fstream>

using namespace std;

bmpDialog::bmpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bmpDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("λͼ����"));
    theData.clear();
    ui->lineEdit_mm->setText(tr("150"));
    ui->lineEdit_pixel->setText(tr("512"));

    QRegExp rx("^(\\d{1,4})$");   //������ʽ:��������4λ����
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->lineEdit_mm->setValidator(pReg);
    ui->lineEdit_pixel->setValidator(pReg);

    ui->progressBar->hide();
    ui->label_save->hide();
    ui->label_saveFinish->hide();
}

bmpDialog::~bmpDialog()
{
    delete ui;
}

void bmpDialog::outlineData(std::vector<xd::outlines> a)
{
    theData.clear();
    theData=a;
    Layer=theData.size();
}

void bmpDialog::on_pushButton_cancel_clicked()
{
    this->close();
}

void bmpDialog::on_pushButton_save_clicked()
{
    bool OK1,OK2;
    int mm=ui->lineEdit_mm->text().toInt(&OK1);
    int pixel=ui->lineEdit_pixel->text().toInt(&OK2);
    if(!(OK1&&OK2&&(mm>=10)&&(pixel>=64)&&(pixel%4==0)))
    {
        QMessageBox::warning(this,tr("����"),tr("������Ϸ����֣�����̨�ߴ�>=10mm������>=64����4�ı���"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    //��ʼ����

    ui->progressBar->setMaximum(Layer+Layer);
    ui->progressBar->setMinimum(0);

    QDateTime time = QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��

    QString str = time.toString(tr("hhʱmm��")); //������ʾ��ʽ
    QString fileName = QFileDialog::getSaveFileName(this,tr("�����ļ�"),str,tr("bmp�ļ�(*.bmp)"));
    if(fileName.length()==0)
        return;

    int PIXEL=pixel;
    vector<vector<vector<int> > > theResult;

    ui->label_saveFinish->hide();
    ui->progressBar->show();
    ui->label_save->show();

    for(int i=0;i!=Layer;++i)
    {
        vector<vector<int> > result1;
        xd::InfillBMP(theData[i],result1,PIXEL,mm);
        theResult.push_back(result1);
        ui->progressBar->setValue(i+1);
    }

    std::string cFileName=q2s(fileName);
    std::string::iterator m=cFileName.end();
    std::string::iterator n=cFileName.end();
    m--;m--;m--;m--;
    cFileName.erase(m,n);
    for(int i=0;i!=Layer;++i)
    {
        char m[3]={0,0,0};
        string cFileNameTem=cFileName+itoa(i+1,m,10);
        cFileNameTem+=".bmp";
        std::ofstream outfile(cFileNameTem.c_str());
        outfile.put('B');
            outfile.put('M');
            int fileSize=54+PIXEL*PIXEL*3;
            outfile.write((char *)&fileSize,4);
            int ApplicationSpecific=0;
            outfile.write((char *)&ApplicationSpecific,4);
            int offsetSize=54;
            outfile.write((char *)&offsetSize,4);
            //DIB Header
            int numberOfDIB=40;
            outfile.write((char *)&numberOfDIB,4);
            int widthOfBmp=PIXEL;
            outfile.write((char *)&widthOfBmp,4);
            int heighOfBmp=PIXEL;
            outfile.write((char *)&heighOfBmp,4);
            short int numberOfColorPlanes=1;
            outfile.write((char *)&numberOfColorPlanes,2);
            short int numberOfBitsPerPixel=24;
            outfile.write((char *)&numberOfBitsPerPixel,2);
            int pixelArrayCompression=0;
            outfile.write((char *)&pixelArrayCompression,4);
            int bitmapDataSize=PIXEL*PIXEL;
            outfile.write((char *)&bitmapDataSize,4);
            int printResolution=(float)PIXEL/mm*1000;   //�ֱ��������ڴ˴������
            //int printResolution=100;
            outfile.write((char *)&printResolution,4);
            outfile.write((char *)&printResolution,4);
            int paletteColorNumbers=0;
            outfile.write((char *)&paletteColorNumbers,4);
            int importantColors=0;
            outfile.write((char *)&importantColors,4);
            //Start of pixel array (bitmap data)
            for (int j=0;j!=PIXEL;++j)
                {
                    for (int k=0;k!=PIXEL;++k)
                    {
                        if (theResult[i][j][k]==0)
                        {
                            int whiteP=0;
                            outfile.write((char *)&whiteP,3);
                        }
                        else
                        {
                            int blackP=16777215;
                            outfile.write((char *)&blackP,3);
                        }
                    }
            }
        ui->progressBar->setValue(Layer+i+1);
    }
    ui->label_save->hide();
    ui->progressBar->hide();
    ui->label_saveFinish->show();
}
