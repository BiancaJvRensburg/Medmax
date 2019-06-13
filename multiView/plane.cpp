#include "plane.h"

Plane::Plane()
{
    position = new Vec(0, 0, 0);
    size = 1.0;

    initBasePlane();
}

void Plane::initBasePlane(){

    points[0] = new Vec(position->x - size, position->y - size, position->z);
    points[1] = new Vec(position->x + size, position->y - size, position->z);
    points[2] = new Vec(position->x + size, position->y + size, position->z);
    points[3] = new Vec(position->x - size, position->y + size, position->z);
}

void Plane::draw(){
    glPushMatrix();
    glMultMatrixd(normalFrame.matrix());

   /* glBegin(GL_QUADS);
    for(int i=0; i<3; i++)
        glVertex3f(points[i]->x, points[i]->y, points[i]->z);
    glEnd();

    */

    glBegin(GL_QUADS);
        glVertex3f(0, - size, - size);
        glVertex3f(0, size, - size);
        glVertex3f(0, size, size);
        glVertex3f(0, - size, size);
    glEnd();

    glPopMatrix();
}

void Plane::movePlane(Vec pos){
    normalFrame.translate(pos);
}
