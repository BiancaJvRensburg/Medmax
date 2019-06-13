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
    Viewer(QWidget *parent, StandardCamera *camera);
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
    //virtual void createPlane(Vec side);
    virtual Vec *initPosition(int side);
    virtual void setMaxDistance(double maxDistance);
    double getMaxDistance();
    /*Vec *rightPos;
    Vec *leftPos;*/
    Axis mainAxis;
    void updateCamera(const Vec3Df & center, float radius);
    //QString filename;
    Plane *leftPlane;
    Plane *rightPlane;
    Curve *curve;

private:
    int zIncL;
    int zIncR;
    int lastPosL;
    int lastPosR;
    double maxDistance;

    Point startPoint;
    Point endPoint;
    int curveIndexL;
    int curveIndexR;
    int nbU;
};

#endif // VIEWER_H
