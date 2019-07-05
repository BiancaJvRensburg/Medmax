#include "curvepoint.h"

CurvePoint::CurvePoint(ManipulatedFrame* mf)
{
    this->mf = mf;
}

void CurvePoint::draw(){

    if(mf->grabsMouse()) glColor3f(0, 1, 1);
    else glColor3f(0.2f, 0.1f, 0.7f);

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
    glEnd();

    glPointSize(1.0);
    glColor3f(0,0,0);

}
