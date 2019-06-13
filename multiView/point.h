#ifndef POINT_H
#define POINT_H

#include <QGLViewer/qglviewer.h>

class Point
{
public:
    Point();
    Point(double x, double y, double z);

    double getX(){ return x;}
    double getY(){ return y;}
    double getZ(){ return z;}

    void setX(double x){this->x = x;}
    void setY(double y){this->y = y;}
    void setZ(double z){this->z = z;}

    qglviewer::Vec toVec(){ return qglviewer::Vec(x, y, z); }

private:
    double x;
    double y;
    double z;
};

#endif // POINT_H
