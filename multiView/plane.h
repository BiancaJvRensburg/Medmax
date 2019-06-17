#ifndef PLANE_H
#define PLANE_H

#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

#include "mesh.h"

using namespace qglviewer;

class Plane
{
public:
    Plane(double s, Axis a);
    void setSize(double s){ size = s; }

    void movePlane(Vec pos);
    void setPosition(const qglviewer::Vec &pos) { normalFrame.setPosition(pos); }
    void setOrientation(Quaternion q){ normalFrame.setOrientation(q); }
    Quaternion getOrientation();
    void draw();

private:
    Vec *position;  // In relation to the normal, this is what will be adjusted with the manipulators
    Vec* points[4];
    double size;
    ManipulatedFrame normalFrame;

    void initBasePlane(Axis a);
};

#endif // PLANE_H
