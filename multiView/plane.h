#ifndef PLANE_H
#define PLANE_H

#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

#include "mesh.h"
#include "curvepoint.h"

using namespace qglviewer;

class Plane
{
public:
    Plane(double s);
    void setSize(double s){ size = s; }

    void setPosition(Vec *pos, double t); // { normalFrame.setPosition(pos); }
    void setOrientation(Quaternion q){ mf->setOrientation(q); }
    Quaternion fromRotatedBasis(Vec x, Vec y, Vec z);

    Vec coordinatesInRef(Vec v){ return mf->coordinatesOfIn(v, mf->referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return mf->coordinatesOfFrom(v, mf->referenceFrame()); }

    void rotatePlaneYZ(double percentage);   // rotate around the z axis
    void rotatePlane(Vec axis, double angle);

    double getT(){ return *t; }
    void draw();

    CurvePoint* cp;

private:
    Vec* position;
    Vec* points[4];
    double size;
    ManipulatedFrame* mf;
    double rotationPercentage;
    void initBasePlane();
    void rotate(Quaternion q) { mf->rotate(q); }
    double* t; // parametre
};

#endif // PLANE_H
