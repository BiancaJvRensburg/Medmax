#include "curvepoint.h"

CurvePoint::CurvePoint(Vec* p) : ControlPoint (p)
{
    //this->p = p;
    this->mf = new ManipulatedFrame();
    //this->curveIndex = curveIndex;
    connect((ManipulatedFrame*)mf, &ManipulatedFrame::manipulated, this, &ControlPoint::cntrlMoved);
}

void CurvePoint::draw(){

    if(((ManipulatedFrame*)mf)->grabsMouse()) glColor3f(0, 1, 1);
    else glColor3f(0.6f, 0, 0.4f);

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
    glEnd();

    glPointSize(1.0);
    glColor3f(1.0,1.0,1.0);

}

void CurvePoint::cntrlMoved(){
    double x,y,z;

    mf->getPosition(x,y,z);

    Vec offset = Vec(x - p->x, y - p->y , z - p->z);

    p->x = x;
    p->y = y;
    p->z = z;

    Q_EMIT CurvePoint::curvePointTranslated(offset);
}
