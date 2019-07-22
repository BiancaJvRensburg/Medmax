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
        /*double percentageL = static_cast<double>(curveIndexL + offset) / static_cast<double>(*nbU);
        double percentageR = static_cast<double>(curveIndexR + offset) / static_cast<double>(*nbU);*/

        leftPlane->setPosition(curve->getPoint(curveIndexL + indexOffset));
        rightPlane->setPosition(curve->getPoint(curveIndexR + indexOffset));
        leftPlane->setOrientation(getNewOrientation(curveIndexL + indexOffset));
        rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

        for(unsigned int i=0; i<ghostPlanes.size(); i++){
            ghostPlanes[i]->setPosition(curve->getPoint(ghostLocation[i] + indexOffset));
            ghostPlanes[i]->setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
        }
    }

    mesh.updatePlaneIntersections();

    update();

}

void ViewerFibula::drawMesh(){

}

void ViewerFibula::addGhostPlanes(int nb){
    ghostPlanes.clear();

    for(unsigned int i=0; i<static_cast<unsigned int>(nb); i++){
        ghostPlanes.push_back(new Plane(40.0, Movable::STATIC));

        ghostPlanes[i]->setPosition(curve->getCurve()[ghostLocation[i] + indexOffset]);
        ghostPlanes[i]->setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
    }

    update();
}

void ViewerFibula::ghostPlanesRecieved(int nb, double distance[]){
    ghostLocation = new int[nb];

    ghostLocation[0] = curve->indexForLength(curveIndexL, distance[0]);
    for(int i=1; i<nb; i++) ghostLocation[i] = curve->indexForLength(ghostLocation[i-1], distance[i]);
    curveIndexR = curve->indexForLength(ghostLocation[nb-1], distance[nb]);

    // doesn't work if its done before the curve is initialised
    addGhostPlanes(nb);
}

void ViewerFibula::getGhostLocations(){

}

void ViewerFibula::movePlaneDistance(double distance){
    if(ghostPlanes.size()==0) curveIndexR = curve->indexForLength(curveIndexL, distance);
    else curveIndexR = curve->indexForLength(ghostLocation[ghostPlanes.size()-1], distance);

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
        ghostPlanes[i]->setPosition(curve->getCurve()[ghostLocation[i] + indexOffset]);
        ghostPlanes[i]->setOrientation(getNewOrientation(ghostLocation[i] + indexOffset));
    }

    curveIndexR += offset;
    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset]);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

    mesh.updatePlaneIntersections(rightPlane);
    update();
}

void ViewerFibula::initCurve(){
    const long nbCP = 4;
    ControlPoint* control[nbCP];

    control[0] = new ControlPoint(102.4, 82.5, -864);
    control[1] = new ControlPoint(91.5, 95.1, -926.9);
    control[2] = new ControlPoint(81.2, 94.4, -1071);
    control[3] = new ControlPoint(80.9, 90.1, -1155);

    curve = new Curve(nbCP, control);

    *nbU = 300;

    int nbSeg = nbCP-3;
    nbU -= *nbU%nbSeg;

    curve->generateCatmull(nbU);
    //curve->generateBSpline(nbU, 3);
    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    initPlanes(Movable::STATIC);

    //addGhostPlanes(2);
}

void ViewerFibula::cutMesh(){
    mesh.setIsCut(Side::EXTERIOR, true);
    update();
}

void ViewerFibula::uncutMesh(){
    mesh.setIsCut(Side::EXTERIOR, false);
    update();
}
