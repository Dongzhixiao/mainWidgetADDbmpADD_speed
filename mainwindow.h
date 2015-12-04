#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget5.h"
#include "generate_line.h"
#include "linescandialog.h"
#include "displaylayerdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_action_O_triggered();

    void on_action_triggered();

    void getInfilledData(std::vector<xd::outlines> a);

    void on_action_2_triggered();

    void get_show_layer(int aa,int bb);

    void on_action_B_triggered();

    void on_action_S_triggered();

private:
    Ui::MainWindow *ui;
    GLWidget5 * displayF;
    int Layer;
    std::vector<xd::outlines> OutlineData;
    std::vector<xd::outlines> InfillData;

signals:
   void passData(std::vector<xd::outlines> OutlineData);
   void passInfillData(std::vector<xd::outlines> a);
   void passDataToLSD(std::vector<xd::outlines> OutlineData);
   void passDataToBMPD(std::vector<xd::outlines> OutlineData);
   void sendLayer(int Layer);
   void passLayer(int lowL,int highL);
};

#endif // MAINWINDOW_H
