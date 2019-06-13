#include "plane.h"

Plane::Plane(double s, Axis a)
{
    position = new Vec(0, 0, 0);
    size = s;

    initBasePlane(a);
}

void Plane::initBasePlane(Axis a){

    switch (a) {
    case Axis::X:
        points[0] = new Vec(position->x, position->y - size, position->z - size);
        points[1] = new Vec(position->x, position->y - size, position->z + size);
        points[2] = new Vec(position->x, position->y + size, position->z + size);
        points[3] = new Vec(position->x, position->y + size, position->z - size);

        break;
    case Axis::Y:
        points[0] = new Vec(position->x - size, position->y, position->z - size);
        points[1] = new Vec(position->x + size, position->y, position->z - size);
        points[2] = new Vec(position->x + size, position->y, position->z + size);
        points[3] = new Vec(position->x - size, position->y, position->z + size);
        break;

    case Axis::Z:
        points[0] = new Vec(position->x - size, position->y - size, position->z);
        points[1] = new Vec(position->x + size, position->y - size, position->z);
        points[2] = new Vec(position->x + size, position->y + size, position->z);
        points[3] = new Vec(position->x - size, position->y + size, position->z);

    }
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

   /* glBegin(GL_QUADS);
        glVertex3f(0, - size, - size);
        glVertex3f(0, size, - size);
        glVertex3f(0, size, size);
        glVertex3f(0, - size, size);
    glEnd();*/

    glPopMatrix();
}

void Plane::movePlane(Vec pos){
    normalFrame.translate(pos);
}
