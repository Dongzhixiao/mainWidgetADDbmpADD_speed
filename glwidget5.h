#ifndef GLWIDGET5_H
#define GLWIDGET5_H

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include "glwidget.h"
#include "generate_line.h"


class GLWidget5 :  public QGLWidget
{

public:
    GLWidget5(int timerInterval = 0, QWidget *parent = 0);
    ~GLWidget5();

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
    GLfloat scaling;
    GLfloat xrot, yrot, zrot;
    GLfloat rtri, rquad;
    GLfloat xmove,ymove;
    QPoint lastPos;
    std::vector<std::vector<std::vector<xd::xdpoint> > > contentTodraw;
    int startL,endL;

public slots:
    void setDrawData(std::vector<xd::outlines> a );
    void setShowLayer(int A, int B);

};

#endif // GLWIDGET5_H
