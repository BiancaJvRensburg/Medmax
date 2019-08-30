#include "plane.h"

Plane::Plane(double s, Movable status)
{
    Vec* position = new Vec(0, 0, 0);
    size = s;
    rotationPercentage = 0;
    normal = Vec(0, 0, 1);
    constraint.setRotationConstraint(AxisPlaneConstraint::AXIS, Vec(0,0,1));
    constraintFree.setRotationConstraint(AxisPlaneConstraint::FREE, Vec(0,0,1));

    this->status = status;

    if(status==Movable::DYNAMIC) cp = new CurvePoint(position);
    else cp = new CurvePoint(position);

    initBasePlane();
}

void Plane::initBasePlane(){
        points[0] = Vec(cp->getPoint().x - size, cp->getPoint().y - size, cp->getPoint().z);
        points[1] = Vec(cp->getPoint().x - size, cp->getPoint().y + size, cp->getPoint().z);
        points[2] = Vec(cp->getPoint().x + size, cp->getPoint().y + size, cp->getPoint().z);
        points[3] = Vec(cp->getPoint().x + size, cp->getPoint().y - size, cp->getPoint().z);
}

void Plane::draw(){
    glPushMatrix();
    glMultMatrixd(cp->getFrame()->matrix());

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
        glVertex3f(static_cast<float>(points[0].x), static_cast<float>(points[0].y), static_cast<float>(points[0].z));
        glVertex3f(static_cast<float>(points[1].x), static_cast<float>(points[1].y), static_cast<float>(points[1].z));
        glVertex3f(static_cast<float>(points[2].x), static_cast<float>(points[2].y), static_cast<float>(points[2].z));
        glVertex3f(static_cast<float>(points[3].x), static_cast<float>(points[3].y), static_cast<float>(points[3].z));
    glEnd();

    if(status==Movable::DYNAMIC) cp->draw();

    glDisable(GL_DEPTH);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1,1,1);
    QGLViewer::drawAxis(15.0);

    glPopMatrix();
}

void Plane::rotatePlane(Vec axis, double theta){
    rotate(Quaternion(cos(theta/2.0)*axis.x, cos(theta/2.0)*axis.y, cos(theta/2.0)*axis.z, sin(theta/2.0)));
}

void Plane::rotatePlaneXY(double percentage){
    // Get the percentage to rotate it by
    double r = (percentage - rotationPercentage);
    rotationPercentage = percentage;

    // Get the theta from the percentage
    double theta = (M_PI*2.0)*r + M_PI;
    Vec axis = Vec(1,0,0);

    rotatePlane(axis, theta);
}

void Plane::setPosition(Vec* pos){
    cp->setPosition(pos);
    cp->getFrame()->setPosition(cp->getX(), cp->getY(), cp->getZ());

    // Only move it if cp is dynamic
    if(status==Movable::DYNAMIC){
        cp->setPosition(pos);
    }
}

// Set the base from a basis x,y,z
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

    // Put it all into local coordinates
    Vec tr0 = cp->getFrame()->localCoordinatesOf(v0);
    Vec tr1 = cp->getFrame()->localCoordinatesOf(v1);
    Vec tr2 = cp->getFrame()->localCoordinatesOf(v2);

    Vec tr[3] = {tr0, tr1, tr2};

    if( (tr0.z < 0 && tr1.z < 0 && tr2.z < 0) || (tr0.z > 0 && tr1.z > 0 && tr2.z > 0) ) return false;  // if they all have the same sign
    else{
        for(int i=0; i<3; i++){

            Vec l = tr[(i+1)%3] - tr[i];

            if(l*normal == 0.0){
                if( tr[i]*normal == 0.0 ){
                    if( (abs(tr[i].x) < size && abs(tr[i].y) < size) || ( abs(tr[(i+1)%3].x) < size && abs(tr[(i+1)%3].y) < size)) return true;  // the plan contains the line
                }
                else continue;  // the line is parallel
            }

            double d = normal*(-tr[i]) / (normal*l);

            if(abs(d) > 1.0) continue;

            Vec intersection = d*l + tr[i];

            if(abs(intersection.x) < size && abs(intersection.y) < size) return true;
        }
    }

    return false;   // if we haven't found a line that meets the criteria
}

double Plane::getSign(Vec v){
    Vec tr0 = cp->getFrame()->localCoordinatesOf(v);

    return tr0.z/(abs(tr0.z));
}

Vec Plane::getProjection(Vec p){
    Vec localP = cp->getFrame()->localCoordinatesOf(p);

    double alpha = (localP * normal);

    Vec newP = localP - normal *alpha;

    return cp->getFrame()->localInverseCoordinatesOf(newP);
}
