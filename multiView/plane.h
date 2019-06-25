#ifndef PLANE_H
#define PLANE_H

#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

#include "mesh.h"

using namespace qglviewer;

class Plane
{
public:
    Plane(double s);
    void setSize(double s){ size = s; }

    void setPosition(Vec *pos); // { normalFrame.setPosition(pos); }
    void setOrientation(Quaternion q){ mf->setOrientation(q); }
    void setOrientationWithConstraint(Quaternion q){ mf->setOrientationWithConstraint(q); }
    void rotate(Quaternion q) { mf->rotate(q); }
    //void setRotation(Vec to, Vec from){ normalFrame.setRotation(Quaternion(to, from));}
    void setRotation(Quaternion q){ mf->setRotationWithConstraint(q);}
    Quaternion fromRotatedBasis(Vec x, Vec y, Vec z);

    Vec coordinatesInRef(Vec v){ return mf->coordinatesOfIn(v, mf->referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return mf->coordinatesOfFrom(v, mf->referenceFrame()); }

    void rotatePlaneZ(double percentage);   // rotate around the z axis
    void rotatePlane(Vec axis, double angle);

    void draw();

private:
    Vec* position;  // In relation to the normal, this is what will be adjusted with the manipulators
    Vec* points[4];
    double size;
    Frame* mf;
    double rotationPercentage;    // Between 0 and 2.0*M_PI  

    AxisPlaneConstraint* constraint;

    void initBasePlane();
};

#endif // PLANE_H
