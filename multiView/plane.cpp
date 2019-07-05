#include "plane.h"

Plane::Plane(double s)
{
    position = new Vec(0, 0, 0);
    size = s;
    rotationPercentage = 0;
    mf = new ManipulatedFrame();

    t = new double();

    *t = 0; // TODO: This is only temporary

    cp = new CurvePoint(position, mf, t);

    initBasePlane();
}

void Plane::initBasePlane(){
        points[0] = new Vec(position->x - size, position->y - size, position->z);
        points[1] = new Vec(position->x - size, position->y + size, position->z);
        points[2] = new Vec(position->x + size, position->y + size, position->z);
        points[3] = new Vec(position->x + size, position->y - size, position->z);
}

void Plane::draw(){
    glPushMatrix();
    glMultMatrixd(mf->matrix());

    glBegin(GL_QUADS);
        glVertex3f(points[0]->x, points[0]->y, points[0]->z);
        glVertex3f(points[1]->x, points[1]->y, points[1]->z);
        glVertex3f(points[2]->x, points[2]->y, points[2]->z);
        glVertex3f(points[3]->x, points[3]->y, points[3]->z);
    glEnd();

    // std::cout << "position " << position->x << " " << position->y << " " << position->z << std::endl;

    cp->draw();

    /*glColor3f(1,1,1);
    QGLViewer::drawAxis(15.0);*/

    glPopMatrix();
}

void Plane::rotatePlane(Vec axis, double theta){
    rotate(Quaternion(cos(theta/2.0)*axis.x, cos(theta/2.0)*axis.y, cos(theta/2.0)*axis.z, sin(theta/2.0)));
}

void Plane::rotatePlaneYZ(double percentage){
    double r = (percentage - rotationPercentage);
    rotationPercentage = percentage;

    double theta = (M_PI*2.0)*r + M_PI;
    Vec axis = Vec(1,0,0);

    rotatePlane(axis, theta);
}

void Plane::setPosition(Vec* pos, double t){
    position = pos;
    mf->setPosition(position->x, position->y, position->z);
    cp->setPosition(pos);

    *this->t = t;
}

Quaternion Plane::fromRotatedBasis(Vec x, Vec y, Vec z){
    Quaternion q = Quaternion();

    q.setFromRotatedBasis(x,y,z);

   return q;
}
