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
    void movePlaneDistance(double, std::vector<Vec>);
    void moveGhostPlaneDistance(double, std::vector<Vec>);
    void ghostPlanesRecieved(int, double[], std::vector<Vec>);
    void middlePlaneMoved(int, double[], std::vector<Vec>);

    void initCurve();

    void cutMesh();
    void uncutMesh();

private:
    void findGhostLocations(int nb, double distance[]); // finds the location of the ghost planes + the right plane
    void setPlaneOrientations(std::vector<Vec> angles);

    int indexOffset;
    int maxOffset;
};

#endif // VIEWERFIBULA_H
