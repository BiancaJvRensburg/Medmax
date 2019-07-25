#ifndef PLANE_H
#define PLANE_H

#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

//#include "mesh.h"
#include "curvepoint.h"

enum Movable {STATIC, DYNAMIC};

using namespace qglviewer;

class Plane
{
public:
    Plane(double s, Movable status);
    void setSize(double s){ size = s; }

    void setPosition(Vec *pos);
    void setOrientation(Quaternion q){ mf->setOrientation(q); }
    Quaternion fromRotatedBasis(Vec x, Vec y, Vec z);

    /*Vec coordinatesInRef(Vec v){ return mf->coordinatesOfIn(v, mf->referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return mf->coordinatesOfFrom(v, mf->referenceFrame()); }*/

    void rotatePlaneYZ(double percentage);   // rotate around the z axis
    void rotatePlane(Vec axis, double angle);

    //double getCurveIndex(){ return *curveIndex; }
    void draw();

    bool isIntersection(Vec v0, Vec v1, Vec v2);
    double getSign(Vec v);

    Vec getProjection(Vec p);
    Vec* getPosition(){ return position; }
    CurvePoint* getCurvePoint(){ return cp; }

    Movable status;
    CurvePoint* cp;

private:
    Vec* position;
    Vec* points[4];
    double size;
    Frame* mf;
    double rotationPercentage;
    void initBasePlane();
    void rotate(Quaternion q) { mf->rotate(q); }
    //double* curveIndex; // parametre
    Vec normal;
};

#endif // PLANE_H
