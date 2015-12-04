#ifndef DISPLAYLAYERDIALOG_H
#define DISPLAYLAYERDIALOG_H

#include <QDialog>

namespace Ui {
class displayLayerDialog;
}

class displayLayerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit displayLayerDialog(QWidget *parent = 0);
    ~displayLayerDialog();
    
private:
    Ui::displayLayerDialog *ui;
    int Layer;

private slots:
    void getLayer(int a);
    void lowChange(int lowN);
    void highChange(int highN);
signals:
    void sendLN(int lN,int hN);
};

#endif // DISPLAYLAYERDIALOG_H
