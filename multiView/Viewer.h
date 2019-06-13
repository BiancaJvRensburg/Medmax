#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include "meshreader.h"
#include "mesh.h"
#include "standardcamera.h"
#include "plane.h"
#include "curve.h"
#include "point.h"

using namespace qglviewer;

class Viewer : public QGLViewer
{
    Q_OBJECT

public :
    Viewer(QWidget *parent, StandardCamera *camera, int sliderMax);
   // ~Viewer();
    void openOFF(QString f);
    Mesh mesh;

public Q_SLOTS:
    virtual void moveLeftPlane(int);
    virtual void moveRightPlane(int);

Q_SIGNALS:
    void leftPosChanged(double);
    void rightPosChanged(double);

protected:
    void draw();
    void init();
    QString helpString() const;
    void wheelEvent(QWheelEvent *e);
    void updateCamera(const Vec3Df & center, float radius);

    virtual void initCurve();
    void initPlanes();

    Plane *leftPlane;
    Plane *rightPlane;
    Curve *curve;

    Point startPoint;
    Point endPoint;
    int curveIndexL;
    int curveIndexR;
    int nbU;
    int sliderMax;

};

#endif // VIEWER_H
