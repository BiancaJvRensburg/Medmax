#include "controlpoint.h"
#include <math.h>

#include <QGLViewer/manipulatedFrame.h>

ControlPoint::ControlPoint(Point* p)
{
    this->p = p;
    initialise();
}

ControlPoint::ControlPoint(double x, double y, double z)
{
    this->p = new Point(x,y,z);
    initialise();
}

void ControlPoint::initialise(){
    mf = new ManipulatedFrame();
    mf->setPosition(this->p->getX(), this->p->getY(), this->p->getZ());
    connect(mf, &ManipulatedFrame::manipulated, this, &ControlPoint::cntrlMoved);
}

void ControlPoint::draw(){

    glPushMatrix();
    glMultMatrixd(mf->matrix());

    if(mf->grabsMouse()) glColor3f(0, 1, 1);
    else glColor3f(1, 0, 1);

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
    glEnd();

    glPointSize(1.0);
    glColor3f(0,0,0);

    glPopMatrix();
}

void ControlPoint::cntrlMoved(){
    double x,y,z;

    mf->getPosition(x,y,z);
    p->setX(x);
    p->setY(y);
    p->setZ(z);

    Q_EMIT ControlPoint::cntrlPointTranslated();
}
