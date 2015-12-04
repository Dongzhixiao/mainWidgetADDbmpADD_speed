#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <glwidget5.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;
    GLWidget5 *displayF;
};

#endif // WIDGET_H
