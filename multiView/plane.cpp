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

    glColor3f(1,1,1);
    QGLViewer::drawAxis(10.0);

    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
        glVertex3f( (points[0]->x + points[1]->x) / 2.0, (points[0]->y + points[1]->y)/2.0, (points[0]->z + points[1]->z)/2.0);
        glVertex3f( (points[2]->x + points[3]->x) / 2.0, (points[2]->y + points[3]->y)/2.0, (points[2]->z + points[3]->z)/2.0);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f( (points[0]->x + points[3]->x) / 2.0, (points[0]->y + points[3]->y)/2.0, (points[0]->z + points[3]->z)/2.0);
        glVertex3f( (points[1]->x + points[2]->x) / 2.0, (points[1]->y + points[2]->y)/2.0, (points[1]->z + points[2]->z)/2.0);
    glEnd();

    glPopMatrix();
}

void Plane::movePlane(Vec pos){
    normalFrame.translate(pos);
}

Quaternion Plane::getOrientation(){
    double w, x, y, z;

    normalFrame.getOrientation(w,x,y,z);
    return Quaternion(w,x,y,z);
}
