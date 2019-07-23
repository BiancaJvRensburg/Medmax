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
    virtual void updatePlanes();
    virtual void cutMesh();
    virtual void uncutMesh();
    virtual void drawMesh();

Q_SIGNALS:
    void leftPosChanged(double);
    void rightPosChanged(double);
    void ghostPlanesAdded(int, double[]);

protected:
    void draw();
    void init();
    QString helpString() const;
    void updateCamera(const Vec3Df & center, float radius);

    virtual void initCurve();
    void initPlanes(Movable status);
    virtual void addGhostPlanes(int nb);
    Quaternion getNewOrientation(int index);
    Quaternion updateOrientation(int index);

    double angle(Vec a, Vec b);
    //static int cmpfunc (const void * a, const void * b);
    int partition(int sorted[], int start, int end);
    void quicksort(int sorted[], int start, int end);

    Plane *leftPlane;
    Plane *rightPlane;
    Curve *curve;
    std::vector<Plane*> ghostPlanes;

    int curveIndexL;
    int curveIndexR;
    long* nbU;
    int sliderMax;
    bool isDrawMesh;
    int* ghostLocation;

    const double constraint = 25;

};

#endif // VIEWER_H
