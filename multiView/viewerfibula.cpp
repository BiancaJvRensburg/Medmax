#include "viewerfibula.h"

ViewerFibula::ViewerFibula(QWidget *parent, StandardCamera *camera, int sliderMax, int fibulaOffset) : Viewer (parent, camera, sliderMax)
{
    indexOffset = 0;
    maxOffset = fibulaOffset;
}

void ViewerFibula::movePlanes(int position){

    int offset = static_cast<int>(static_cast<double>(position)/ static_cast<double>(maxOffset) * static_cast<double>(nbU));

    if(curveIndexL + offset < nbU && curveIndexL + offset > 0 && curveIndexR + offset < nbU && curveIndexR + offset > 0){
        indexOffset = offset;
        double percentageL = static_cast<double>(curveIndexL + offset) / static_cast<double>(nbU);
        double percentageR = static_cast<double>(curveIndexR + offset) / static_cast<double>(nbU);

        leftPlane->setPosition(curve->getPoint(curveIndexL + indexOffset), percentageL);
        rightPlane->setPosition(curve->getPoint(curveIndexR + indexOffset), percentageR);
        leftPlane->setOrientation(getNewOrientation(curveIndexL + indexOffset));
        rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));
    }

    update();

}

void ViewerFibula::movePlaneDistance(double distance){
    curveIndexR = curve->indexForLength(curveIndexL, distance);
    double percentage = static_cast<double>(curveIndexR + indexOffset) / static_cast<double>(nbU);
    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset], percentage);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));
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

    nbU = 1000;

    curve->generateBSpline(nbU, 3);
    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    initPlanes();
}
