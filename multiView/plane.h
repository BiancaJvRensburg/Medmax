#ifndef PLANE_H
#define PLANE_H

#include <QGLViewer/qglviewer.h>

using namespace qglviewer;

class Plane
{
public:
    Plane();
    Plane(Vec* position, Vec* normal);

    Vec* getPosition(){ return position; }
    Vec* getNormal(){ return normal; }
    void setPosition(Vec* pos){ position = pos; }
    void setNormal(Vec* norm){ normal = norm; }
    void setSize(double s){ size = s; }

    void movePlane(Vec* pos, Vec* norm);

private:
    Vec *position;
    Vec *normal;
    Vec *normalRotation;
    Vec *positionTranslation;
    Vec* points[4];
    double size;

    void initBasePlane();
    void adjustPointsNormal();
    void adjustPointsPosition();
    void movePosition(double x, double y, double z);
    void adjustNormal(double x, double y, double z);

    void draw();
};

#endif // PLANE_H
