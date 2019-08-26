#ifndef VIEWERFIBULA_H
#define VIEWERFIBULA_H

#include "viewer.h"

class ViewerFibula : public Viewer
{
    Q_OBJECT

public:
    ViewerFibula(QWidget *parent, StandardCamera *camera, int sliderMax, int fibulaOffsetMax);
    void addGhostPlanes(int nb);
    void handleCut();

public Q_SLOTS:
    void movePlanes(int);
    void movePlaneDistance(double, std::vector<Vec>);
    void moveGhostPlaneDistance(double, std::vector<Vec>);
    void ghostPlanesRecieved(int, double[], std::vector<Vec>);
    void middlePlaneMoved(int, double[], std::vector<Vec>);

    void initCurve();
    void cutMesh();
    void uncutMesh();

Q_SIGNALS:
    void setPlaneSliderValue(int);

private:
    void findGhostLocations(int nb, double distance[]); // finds the location of the ghost planes + the right plane
    void setPlaneOrientations(std::vector<Vec> angles);
    void reinitialisePlanes(unsigned int nbToInit);      // Reinitialises the position and orientation of the planes

    bool isCutSignal;
    bool isPlanesRecieved;

    int indexOffset;
    int maxOffset;
    std::vector<Vec> angleVectors;  // the last set of orientation vectors we recieved (kept for when we want to slide the planes along the fibula)
};

#endif // VIEWERFIBULA_H
