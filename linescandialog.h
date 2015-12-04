#ifndef LINESCANDIALOG_H
#define LINESCANDIALOG_H

#include <QDialog>
#include "generate_line.h"

namespace Ui {
class lineScanDialog;
}

class lineScanDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit lineScanDialog(QWidget *parent = 0);
    ~lineScanDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_confirm_clicked();

    void outlineData(std::vector<xd::outlines> a);

    void on_pushButton_save_clicked();

private:
    Ui::lineScanDialog *ui;
    std::vector<xd::outlines> theData;  //接收的slc的轮廓数据
    std::vector<xd::outlines> InfilledData;  //存放填充后的数据
    xd::outlines firstResult; //第一层数据
    xd::outlines secondResult; //第二层数据
    std::vector<float> recordZ;  //记录加入底座前每一层的高度
signals:
    void sendInfilledData(std::vector<xd::outlines> InfilledData);
};

#endif // LINESCANDIALOG_H
