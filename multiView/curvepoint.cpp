#include "curvepoint.h"

CurvePoint::CurvePoint(Vec* p, ManipulatedFrame* mf, double* t)
{
    this->p = p;
    this->mf = mf;
    this->t = t;
    connect(mf, &ManipulatedFrame::manipulated, this, &ControlPoint::cntrlMoved);
}

void CurvePoint::draw(){

    if(mf->grabsMouse()) glColor3f(0, 1, 1);
    else glColor3f(0.2f, 0.1f, 0.7f);

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
    glEnd();

    /*double x,y,z;

    mf->getPosition(x,y,z);

    std::cout << x << " " << y << " " << z << std::endl;*/

    glPointSize(1.0);
    glColor3f(0,0,0);

}

void CurvePoint::cntrlMoved(){
    double x,y,z;

    mf->getPosition(x,y,z);

   // std::cout << "MOVED    " << x << " " << y << " " << z << std::endl;
    Vec offset = Vec(x - p->x, y - p->y , z - p->z);

    //std::cout << "-- offset " << offset.x << " " << offset.y << " " << offset.z << std::endl;

    // std::cout << "-- prev pos : " << x - offset.x << " " << y - offset.y << " " << z - offset.z << std::endl;

    p->x = x;
    p->y = y;
    p->z = z;

    // std::cout << *this->t << std::endl;

    Q_EMIT CurvePoint::curvePointTranslated(offset, *this->t);
}