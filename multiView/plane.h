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

    void setPosition(Vec *pos); // { normalFrame.setPosition(pos); }
    void setOrientation(Quaternion q){ mf->setOrientation(q); }
    Quaternion fromRotatedBasis(Vec x, Vec y, Vec z);

    Vec coordinatesInRef(Vec v){ return mf->coordinatesOfIn(v, mf->referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return mf->coordinatesOfFrom(v, mf->referenceFrame()); }

    void rotatePlaneYZ(double percentage);   // rotate around the z axis
    void rotatePlane(Vec axis, double angle);

    void draw();

private:
    CurvePoint* cp;
    Vec* position;
    Vec* points[4];
    double size;
    ManipulatedFrame* mf;
    double rotationPercentage;
    void initBasePlane();
    void rotate(Quaternion q) { mf->rotate(q); }
};

#endif // PLANE_H
