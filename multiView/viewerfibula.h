#ifndef VIEWERFIBULA_H
#define VIEWERFIBULA_H

#include "viewer.h"

class ViewerFibula : public Viewer
{
    Q_OBJECT

public:
    ViewerFibula(QWidget *parent, StandardCamera *camera, int sliderMax, int fibulaOffsetMax);
    void addGhostPlanes(int nb);

public Q_SLOTS:
    void movePlanes(int);
    void movePlaneDistance(double);
    void moveGhostPlaneDistance(double);
    void ghostPlanesRecieved(int, double[]);

    void initCurve();

    void cutMesh();
    void uncutMesh();
    void drawMesh();

private:
    int indexOffset;
    int maxOffset;
};

#endif // VIEWERFIBULA_H
