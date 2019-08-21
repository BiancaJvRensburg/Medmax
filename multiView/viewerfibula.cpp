#include "viewerfibula.h"

ViewerFibula::ViewerFibula(QWidget *parent, StandardCamera *camera, int sliderMax, int fibulaOffset) : Viewer (parent, camera, sliderMax)
{
    indexOffset = 0;
    maxOffset = fibulaOffset;
}

void ViewerFibula::movePlanes(int position){

    int offset = static_cast<int>(static_cast<double>(position)/ static_cast<double>(maxOffset) * static_cast<double>(*nbU));

    if(curveIndexL + offset < *nbU && curveIndexL + offset > 0 && curveIndexR + offset < *nbU && curveIndexR + offset > 0){
        indexOffset = offset;

        leftPlane->setPosition(curve->getPoint(curveIndexL + indexOffset));
        rightPlane->setPosition(curve->getPoint(curveIndexR + indexOffset));
        leftPlane->setOrientation(getNewOrientation(curveIndexL + indexOffset));
        rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

        for(unsigned int i=0; i<ghostPlanes.size(); i++){
            ghostPlanes[i].setPosition(curve->getPoint(ghostLocation[i] + indexOffset));
            ghostPlanes[i].setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
        }
    }

    mesh.updatePlaneIntersections();

    update();

}

void ViewerFibula::addGhostPlanes(int nb){
    ghostPlanes.clear();

    for(unsigned int i=0; i<static_cast<unsigned int>(nb); i++){
        ghostPlanes.push_back(Plane(40.0, Movable::STATIC));

        ghostPlanes[i].setPosition(curve->getCurve()[ghostLocation[i] + indexOffset]);
        ghostPlanes[i].setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
    }

    update();
}

void ViewerFibula::findGhostLocations(int nb, double distance[]){
    ghostLocation.clear();

    int index = curve->indexForLength(curveIndexL, distance[0]);
    ghostLocation.push_back(index);
    ghostLocation.push_back(index+5);
    for(int i=1; i<nb; i++){
        index = curve->indexForLength(ghostLocation[2*i-1], distance[i]);
        ghostLocation.push_back(index);
        int nbU = curve->getNbU();
        if((index+5)<nbU) ghostLocation.push_back(index+5);
        else ghostLocation.push_back(nbU-1);
    }
    curveIndexR = curve->indexForLength(ghostLocation[2*nb-1], distance[nb]);
}

void ViewerFibula::ghostPlanesRecieved(int nb, double distance[]){
    if(nb==0){
        ghostPlanes.clear();
        return;
    }

    findGhostLocations(nb, distance);

    // doesn't work if its done before the curve is initialised (should never happen)
    addGhostPlanes(2*nb);
}

void ViewerFibula::movePlaneDistance(double distance){
    int newIndex;
    if(ghostPlanes.size()==0) newIndex = curve->indexForLength(curveIndexL, distance);
    else newIndex = curve->indexForLength(ghostLocation[ghostPlanes.size()-1], distance);

    if(newIndex + indexOffset >= *nbU) return;      // This should never happen
    else curveIndexR = newIndex;

    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset]);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));
    mesh.updatePlaneIntersections(rightPlane);
    update();
}

void ViewerFibula::moveGhostPlaneDistance(double distance){
    int offset = 0;
    if(ghostPlanes.size()==0) movePlaneDistance(distance);
    else offset = curve->indexForLength(curveIndexL, distance) - ghostLocation[0];

    // move all planes by the offset
    for(unsigned int i=0; i<ghostPlanes.size(); i++){
        ghostLocation[i] += offset;
        ghostPlanes[i].setPosition(curve->getCurve()[ghostLocation[i] + indexOffset]);
        ghostPlanes[i].setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
    }

    curveIndexR += offset;
    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset]);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

    mesh.updatePlaneIntersections(rightPlane);
    update();
}

void ViewerFibula::middlePlaneMoved(int nb, double distances[]){
    if(nb==0) return;

    findGhostLocations(nb, distances);

    // update the ghost planes
    for(unsigned int i=0; i<static_cast<unsigned int>(2*nb); i++){
        ghostPlanes[i].setPosition(curve->getCurve()[ghostLocation[i] + indexOffset]);
        ghostPlanes[i].setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
    }

    // update the right plane
    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset]);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

    // update the mesh intersections
    mesh.updatePlaneIntersections(rightPlane);

    update();
}

void ViewerFibula::initCurve(){
    const long nbCP = 6;
    ControlPoint* control[nbCP];

    control[0] = new ControlPoint(108.241, 69.6891, -804.132);

    control[1] = new ControlPoint(97.122, 82.1788, -866.868);
    control[2] = new ControlPoint(93.5364, 90.1045, -956.126);
    control[3] = new ControlPoint(83.3966, 92.5807, -1069.7);
    control[4] = new ControlPoint(80.9, 90.1, -1155);

    control[5] = new ControlPoint(86.4811, 90.9929, -1199.7);

    curve = new Curve(nbCP, control);

    *nbU = 300;

    int nbSeg = nbCP-3;
    nbU -= *nbU%nbSeg;

    curve->generateCatmull(nbU);
    //curve->generateBSpline(nbU, 3);
    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    initPlanes(Movable::STATIC);
}

void ViewerFibula::cutMesh(){
    mesh.setIsCut(Side::EXTERIOR, true);
    isGhostPlanes = true;
    update();
}

void ViewerFibula::uncutMesh(){
    mesh.setIsCut(Side::EXTERIOR, false);
    isGhostPlanes = false;
    ghostPlanes.clear();
    update();
}
