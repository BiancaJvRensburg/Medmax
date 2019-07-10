#include "plane.h"

Plane::Plane(double s, Movable status)
{
    position = new Vec(0, 0, 0);
    size = s;
    rotationPercentage = 0;

    this->status = status;

    t = new double();

    if(status==Movable::DYNAMIC){
        mf = new ManipulatedFrame();
        *t = 0; // TODO: This is only temporary
        cp = new CurvePoint(position, (ManipulatedFrame*)mf, t);
    }
    else{
        mf = new Frame();
        *t = 0;
        cp = NULL;
    }

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
        glVertex3f(static_cast<float>(points[0]->x), static_cast<float>(points[0]->y), static_cast<float>(points[0]->z));
        glVertex3f(static_cast<float>(points[1]->x), static_cast<float>(points[1]->y), static_cast<float>(points[1]->z));
        glVertex3f(static_cast<float>(points[2]->x), static_cast<float>(points[2]->y), static_cast<float>(points[2]->z));
        glVertex3f(static_cast<float>(points[3]->x), static_cast<float>(points[3]->y), static_cast<float>(points[3]->z));
    glEnd();

    /*glColor3f(1,1,1);
    QGLViewer::drawAxis(15.0);*/

    if(status==Movable::DYNAMIC) cp->draw();

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

    if(status==Movable::DYNAMIC){
        cp->setPosition(pos);
        *this->t = t;
    }
}

Quaternion Plane::fromRotatedBasis(Vec x, Vec y, Vec z){
    Quaternion q = Quaternion();

    q.setFromRotatedBasis(x,y,z);

   return q;
}

// MESH INTERSECTION

/*
* Pass the vertices to check in the form of Vec
* The mesh will then use this information
* This is the only thing that the plane deals with
*/
bool Plane::isIntersection(Vec v0, Vec v1, Vec v2){
    Vec tr0 = mf->localCoordinatesOf(v0);
    Vec tr1 = mf->localCoordinatesOf(v1);
    Vec tr2 = mf->localCoordinatesOf(v2);

    if( (tr0.z < 0 && tr1.z < 0 && tr2.z < 0) || (tr0.z > 0 && tr1.z > 0 && tr2.z > 0) ) return false;

    return true;
}
