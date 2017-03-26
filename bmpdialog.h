#ifndef BMPDIALOG_H
#define BMPDIALOG_H

#include <QDialog>
#include "generate_line.h"

namespace Ui {
class bmpDialog;
}

class bmpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit bmpDialog(QWidget *parent = 0);
    ~bmpDialog();
    
private:
    Ui::bmpDialog *ui;
    std::vector<xd::outlines> theData;  //接收的slc的轮廓数据
    int Layer;
private slots:
    void outlineData(std::vector<xd::outlines> a);
    void on_pushButton_cancel_clicked();
    void on_pushButton_save_clicked();
};

#endif // BMPDIALOG_H
