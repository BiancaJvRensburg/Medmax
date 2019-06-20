#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "point.h"
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

using namespace qglviewer;

class ControlPoint
{
public:
    ControlPoint(){ p = new Point(0,0,0);}
    ControlPoint(Point* p);
    ControlPoint(double x, double y, double z);

    Point* getPoint();
    double getX();
    double getY();
    double getZ();

    void draw();

private:
    Point* p;
    ManipulatedFrame mf;
};

#endif // CONTROLPOINT_H
