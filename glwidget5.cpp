#include "glwidget5.h"
#include <GL/glu.h>

GLWidget5::GLWidget5(int timerInterval, QWidget *parent)
    : QGLWidget( parent)
{
    scaling = 1.0;
    xrot = yrot = zrot = 0.0;
    rtri = rquad = 0.0f;
    xmove=ymove=0.0f;
    contentTodraw.clear();
	//contentTodraw.clear();
    //setWindowTitle(tr("3D�ռ�"));
    //QIcon icon(":/images/war4.png");
   // setWindowIcon(icon);
   // showMaximized();

}

void GLWidget5::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void GLWidget5::resizeGL(int width, int height)
{
    height = height?height:1;
    glViewport( 0, 0, (GLint)width, (GLint)height );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget5::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����Ļ����Ȼ���
    glLoadIdentity(); // ���õ�ǰ��ģ�͹۲����

    glTranslatef(-1.5f,0.0f,-6.0f); // ��Ե�ǰ���ڵ���Ļλ���ƶ�
    glScalef(scaling, scaling, scaling);
    glTranslatef(xmove/scaling,ymove/scaling,0);
    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);

    glRotatef(rtri, 0.0f, 1.0f, 0.0f); // ��y����ת������

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

}

void GLWidget5::timeOut()
{
//    rtri += 0.5f;
//    rquad -= 0.25f;
    updateGL();
}

void GLWidget5::keyPressEvent(QKeyEvent *e)
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

void GLWidget5::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
}

void GLWidget5::mouseMoveEvent(QMouseEvent *e)
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

void GLWidget5::wheelEvent(QWheelEvent *e)
{
    double numDegrees = -e->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    scaling /= pow(1.125, numSteps);
    updateGL();
}


void GLWidget5::setDrawData(std::vector<xd::outlines> a)
{
    //std::cout<<"������slot"<<endl;
    contentTodraw.clear();
    contentTodraw=a;
    startL=1;
    endL=a.size();
    //std::cout<<contentTodraw[1][1][1].x<<endl;
    updateGL();
}

void GLWidget5::setShowLayer(int A, int B)
{
    startL=A;
    endL=B;
    updateGL();
}

GLWidget5::~GLWidget5()
{
     contentTodraw.clear();
}