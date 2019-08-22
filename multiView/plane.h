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
    //virtual ~Plane();
    ~Plane(){
        //std::cout << "destroying plane " << this << std::endl;
        //delete cp;
    }
    void setSize(double s){ size = s; }

    void setPosition(Vec *pos);
    void setOrientation(Quaternion q){ cp->getFrame()->setOrientation(q); }
    Quaternion fromRotatedBasis(Vec x, Vec y, Vec z);

    /*Vec coordinatesInRef(Vec v){ return mf->coordinatesOfIn(v, mf->referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return mf->coordinatesOfFrom(v, mf->referenceFrame()); }*/

    Vec localCoordinatesOf(Vec v){ cp->getFrame()->localCoordinatesOf(v); }
    Vec localInverseCoordinatesOf(Vec v){ cp->getFrame()->localInverseCoordinatesOf(v); }
    Vec worldCoordinatesOf(Vec v){ cp->getFrame()->inverseCoordinatesOf(v); }
    Vec worldTransformOf(Vec v){ cp->getFrame()->inverseTransformOf(v); }
    Vec localCoordinatesFrom(Vec v, const Frame *f){ cp->getFrame()->coordinatesOfFrom(v, f); }
    Vec coordinatesInFrame(Vec v, const Frame *f){ cp->getFrame()->coordinatesOfIn(v, f); }

    Frame* getFrame(){ return cp->getFrame(); }

    double getIntersectionAngle(Vec v); // angle between the plane normal and the polyline
    Vec getPolylineVector(Vec v);   // Return the vector v in the coordinates of this plane (could be done w/ another function)

    void rotatePlaneYZ(double percentage);   // rotate around the z axis
    void rotatePlane(Vec axis, double angle);
    void rotateNormal(double angle);

    //double getCurveIndex(){ return *curveIndex; }
    void draw();

    bool isIntersection(Vec v0, Vec v1, Vec v2);
    double getSign(Vec v);

    Vec getProjection(Vec p);
    Vec* getPosition(){ return cp->getPoint(); }
    CurvePoint* getCurvePoint(){ return cp; }

    Movable status;

private:
    //Vec* position;
    Vec points[4];
    double size;
    //Frame* mf;
    double rotationPercentage;
    void initBasePlane();
    void rotate(Quaternion q) { cp->getFrame()->rotate(q); }
    //double* curveIndex; // parametre
    Vec normal;
    CurvePoint* cp;
};

#endif // PLANE_H
