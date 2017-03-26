#ifndef SKINCOREDIALOG_H
#define SKINCOREDIALOG_H

#include <QDialog>
#include "generate_line.h"

namespace Ui {
class skinCoreDialog;
}

class skinCoreDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit skinCoreDialog(QWidget *parent = 0);
    ~skinCoreDialog();
    
private:
    Ui::skinCoreDialog *ui;
    std::vector<xd::outlines> theData;  //接收的slc的轮廓数据
    std::vector<xd::outlines> InfilledData;  //存放填充后的数据

private slots:
    void outlineData(std::vector<xd::outlines> a);
    void on_pushButton_confirm_clicked();
};

#endif // SKINCOREDIALOG_H
