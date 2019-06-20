#include "controlpoint.h"

ControlPoint::ControlPoint(Point* p)
{
    this->p = p;
    mf.setPosition(this->p->getX(), this->p->getY(), this->p->getZ());
}

ControlPoint::ControlPoint(double x, double y, double z)
{
    this->p = new Point(x,y,z);
    mf.setPosition(x,y,z);
}

void ControlPoint::draw(){

    glPushMatrix();
    glMultMatrixd(mf.matrix());

    if(mf.grabsMouse()) glColor3f(0, 1, 1);
    else glColor3f(1, 0, 1);

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
    glEnd();

    glPointSize(1.0);
    glColor3f(0,0,0);

    glPopMatrix();
}

Point* ControlPoint::getPoint(){
    return p;
}

double ControlPoint::getX(){
   return p->getX();
}

double ControlPoint::getY(){

    return p->getY();
}

double ControlPoint::getZ(){

    return p->getZ();
}
