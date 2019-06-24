#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include "meshreader.h"
#include "mesh.h"
#include "standardcamera.h"
#include "plane.h"
#include "curve.h"

using namespace qglviewer;

class Viewer : public QGLViewer
{
    Q_OBJECT

public :
    Viewer(QWidget *parent, StandardCamera *camera, int sliderMax);
    void openOFF(QString f);
    Mesh mesh;

public Q_SLOTS:
    virtual void moveLeftPlane(int);
    virtual void moveRightPlane(int);
    virtual void rotateLeftPlane(int);
    virtual void rotateRightPlane(int);

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
    Quaternion getNewOrientation(int index);
    void updatePlane();

    Plane *leftPlane;
    Plane *rightPlane;
    Curve *curve;

    Vec startPoint;
    Vec endPoint;
    int curveIndexL;
    int curveIndexR;
    int nbU;
    int sliderMax;

    double angle(Vec a, Vec b);

};

#endif // VIEWER_H
