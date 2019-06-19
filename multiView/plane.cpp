#include "plane.h"

Plane::Plane(double s)
{
    position = new Vec(0, 0, 0);
    size = s;

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

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex3f(points[0]->x, points[0]->y, points[0]->z);
        glVertex3f(points[1]->x, points[1]->y, points[1]->z);
        glVertex3f(points[2]->x, points[2]->y, points[2]->z);
        glVertex3f(points[3]->x, points[3]->y, points[3]->z);
    glEnd();

    QGLViewer::drawAxis(15.0);

    /*glColor3f(0, 0, 0);
    glBegin(GL_LINES);
        glVertex3f( (points[0]->x + points[1]->x) / 2.0, (points[0]->y + points[1]->y)/2.0, (points[0]->z + points[1]->z)/2.0);
        glVertex3f( (points[2]->x + points[3]->x) / 2.0, (points[2]->y + points[3]->y)/2.0, (points[2]->z + points[3]->z)/2.0);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f( (points[0]->x + points[3]->x) / 2.0, (points[0]->y + points[3]->y)/2.0, (points[0]->z + points[3]->z)/2.0);
        glVertex3f( (points[1]->x + points[2]->x) / 2.0, (points[1]->y + points[2]->y)/2.0, (points[1]->z + points[2]->z)/2.0);
    glEnd();*/

    /*Quaternion q = getOrientation();
    Vec axe = Vec(position->x+q.axis().x*100.0, position->y+q.axis().y*100.0, position->z+q.axis().z*100.0);

    glColor3f(1.0,0.5,0.0);
    glBegin(GL_LINES);
        glVertex3f(position->x, position->y, position->z);
        glVertex3f(axe.x, axe.y, axe.z);
    glEnd();*/

    glPopMatrix();
}

void Plane::movePlane(Vec pos){
    normalFrame.translate(pos);
}
