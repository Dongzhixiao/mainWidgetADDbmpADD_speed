#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QWidget>
#include "generate_line.h"


namespace Ui {
class glwidget;
}

class glwidget : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit glwidget(QWidget *parent = 0);
    ~glwidget();

protected:
    virtual void initializeGL();
    virtual void resizeGL( int width, int height );
    virtual void paintGL();
    virtual void timeOut();
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    Ui::glwidget *ui;
    GLfloat scaling;
    GLfloat xrot, yrot, zrot;
    GLfloat rtri, rquad;
    GLfloat xmove,ymove;
    QPoint lastPos;
    std::vector<std::vector<std::vector<xd::xdpoint> > > infillTodraw;
    std::vector<std::vector<std::vector<xd::xdpoint> > > contentTodraw;
    int startL,endL;

public slots:
    void setDrawData(std::vector<xd::outlines> a );
    void setInfillDrawData(std::vector<xd::outlines> a );
    void setShowLayer(int A, int B);

};

#endif // GLWIDGET_H
