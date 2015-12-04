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
    std::vector<xd::outlines> theData;  //���յ�slc����������
    std::vector<xd::outlines> InfilledData;  //������������
    xd::outlines firstResult; //��һ������
    xd::outlines secondResult; //�ڶ�������
    std::vector<float> recordZ;  //��¼�������ǰÿһ��ĸ߶�
signals:
    void sendInfilledData(std::vector<xd::outlines> InfilledData);
};

#endif // LINESCANDIALOG_H
