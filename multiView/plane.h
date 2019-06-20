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

    void movePlane(Vec pos);

    void setPosition(const qglviewer::Vec &pos) { normalFrame.setPosition(pos); }
    void setOrientation(Quaternion q){ normalFrame.setOrientation(q); }
    void rotate(Quaternion q) { normalFrame.rotate(q); }
    //void setRotation(Vec to, Vec from){ normalFrame.setRotation(Quaternion(to, from));}
    void setRotation(Quaternion q){ normalFrame.setRotation(q);}

    Vec coordinatesInRef(Vec v){ return normalFrame.coordinatesOfIn(v, normalFrame.referenceFrame()); }
    Vec refCoordinatesInFrame(Vec v){ return normalFrame.coordinatesOfFrom(v, normalFrame.referenceFrame()); }

    void rotatePlaneZ(double percentage);   // rotate around the z axis

    void draw();

private:
    Vec *position;  // In relation to the normal, this is what will be adjusted with the manipulators
    Vec* points[4];
    double size;
    ManipulatedFrame normalFrame;
    double rotationPercentage;    // Between 0 and 2.0*M_PI

    void initBasePlane();
};

#endif // PLANE_H
