#include "plane.h"

Plane::Plane(double s)
{
    position = new Vec(0, 0, 0);
    size = s;
    rotationPercentage = 0;

    initBasePlane();
}

void Plane::initBasePlane(){
        points[0] = new Vec(position->x - size, position->y, position->z - size);
        points[1] = new Vec(position->x + size, position->y, position->z - size);
        points[2] = new Vec(position->x + size, position->y, position->z + size);
        points[3] = new Vec(position->x - size, position->y, position->z + size);
}

void Plane::draw(){
    glPushMatrix();
    glMultMatrixd(normalFrame.matrix());

    glBegin(GL_QUADS);
        glVertex3f(points[0]->x, points[0]->y, points[0]->z);
        glVertex3f(points[1]->x, points[1]->y, points[1]->z);
        glVertex3f(points[2]->x, points[2]->y, points[2]->z);
        glVertex3f(points[3]->x, points[3]->y, points[3]->z);
    glEnd();

    glColor3f(1,1,1);
    QGLViewer::drawAxis(15.0);

    glPopMatrix();
}

void Plane::rotatePlaneZ(double percentage){
    double r = (percentage - rotationPercentage);
        rotationPercentage = percentage;

        double theta = (M_PI*2.0)*r + M_PI;
        rotate(Quaternion(0,0,cos(theta/2.0), sin(theta/2.0)));
}

void Plane::movePlane(Vec pos){
    normalFrame.translate(pos);
}
