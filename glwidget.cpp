#include "glwidget.h"
#include "ui_glwidget.h"
#include <GL/glut.h>

glwidget::glwidget(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::glwidget)
{
    ui->setupUi(this);
    scaling = 1.0;
    xrot = yrot = zrot = 0.0;
    rtri = rquad = 0.0f;
    xmove=ymove=0.0f;
    contentTodraw.clear();
    infillTodraw.clear();
}

glwidget::~glwidget()
{
    delete ui;
}

void glwidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void glwidget::resizeGL(int width, int height)
{
    height = height?height:1;
    glViewport( 0, 0, (GLint)width, (GLint)height );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glwidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存
    glLoadIdentity(); // 重置当前的模型观察矩阵

    glTranslatef(-1.5f,0.0f,-6.0f); // 相对当前所在的屏幕位置移动
    glScalef(scaling, scaling, scaling);
    glTranslatef(xmove/scaling,ymove/scaling,0);
    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);

    glRotatef(rtri, 0.0f, 1.0f, 0.0f); // 绕y轴旋转金字塔

    if(!contentTodraw.empty())
    {
        for(int i=startL-1;i!=endL;++i)
        {
            for(int j=0;j!=contentTodraw[i].size();++j)
            {
                glBegin(GL_LINE_STRIP);
                for(int k=0;k!=contentTodraw[i][j].size();++k)
                {
                    glColor3f(1.0f,0.0f,0.0f);

                    glVertex3f(contentTodraw[i][j][k].x/50,contentTodraw[i][j][k].y/50,contentTodraw[i][j][k].z/50);
                }
                glEnd();
            }
        }
    }
    if(!infillTodraw.empty())
    {
        for(int i=startL-1;i!=endL;++i)
        {
            for(int j=0;j!=infillTodraw[i].size();++j)
            {
                glBegin(GL_LINE_STRIP);
                for(int k=0;k!=infillTodraw[i][j].size();++k)
                {
                    glColor3f(1.0f,1.0f,1.0f);

                    glVertex3f(infillTodraw[i][j][k].x/50,infillTodraw[i][j][k].y/50,infillTodraw[i][j][k].z/50);
                }
                glEnd();
            }
        }
    }

}

void glwidget::timeOut()
{
//   rtri += 0.5f;
//    rquad -= 0.25f;
    updateGL();
}

void glwidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_F11:
        if (isFullScreen())
            showNormal();
        else
            showFullScreen();
        break;
    case Qt::Key_Escape:
            close();
    }
}

void glwidget::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
}

void glwidget::mouseMoveEvent(QMouseEvent *e)
{
    GLfloat dx = GLfloat(e->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(e->y() - lastPos.y()) / height();
    if (e->buttons() & Qt::LeftButton) {
        xrot += 180 * dy;
        yrot += 180 * dx;
        updateGL();
    } else if (e->buttons() & Qt::RightButton) {
        xrot -= 180 * dy;
        zrot -= 180 * dx;
        updateGL();
    }
    else if(e->buttons() & Qt::MidButton)
    {
        xmove+=dx*7;
        ymove-=dy*5;
        updateGL();
    }
    lastPos = e->pos();
}

void glwidget::wheelEvent(QWheelEvent *e)
{
    double numDegrees = -e->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    scaling /= pow(1.125, numSteps);
    updateGL();
}


void glwidget::setDrawData(std::vector<xd::outlines> a)
{
    //std::cout<<"进入了slot"<<endl;
    infillTodraw.clear();
    contentTodraw.clear();
    contentTodraw=a;
    startL=1;
    endL=a.size();
    //std::cout<<contentTodraw[1][1][1].x<<endl;
    updateGL();
}

void glwidget::setInfillDrawData(std::vector<xd::outlines> a )
{
    infillTodraw=a;
    updateGL();
}
void glwidget::setShowLayer(int A, int B)
{
    startL=A;
    endL=B;
    updateGL();
}
