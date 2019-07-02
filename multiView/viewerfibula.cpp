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
        leftPlane->setPosition(curve->getPoint(curveIndexL + indexOffset));
        rightPlane->setPosition(curve->getPoint(curveIndexR + indexOffset));
        leftPlane->setOrientation(getNewOrientation(curveIndexL + indexOffset));
        rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));
    }

    update();

}

void ViewerFibula::moveLeftPlane(double percentage){

    curveIndexL = static_cast<int>(percentage * static_cast<double>(nbU) );

    if(curveIndexL + indexOffset >= nbU) curveIndexL = nbU-1 - indexOffset;
    else if(curveIndexL + indexOffset < 0) curveIndexL = - indexOffset;

    leftPlane->setPosition(curve->getCurve()[curveIndexL + indexOffset]);
    leftPlane->setOrientation(getNewOrientation(curveIndexL + indexOffset));

    update();
}

void ViewerFibula::moveRightPlane(double percentage){

    curveIndexR = nbU - 1 - static_cast<int>(percentage * static_cast<double>(nbU) );

    if(curveIndexR >= nbU) curveIndexR = nbU-1;
    else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen

    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset]);
    rightPlane->setOrientation(getNewOrientation(curveIndexR + indexOffset));

    update();
}

void ViewerFibula::initCurve(){
    const long nbCP = 4;
    ControlPoint* control[nbCP];

    /*control[0] = new ControlPoint(70, 70, -800);
    control[1] = new ControlPoint(80, 100, -900);
    control[2] = new ControlPoint(75, 95, -1100);
    control[3] = new ControlPoint(80, 90, -1200);*/

    control[0] = new ControlPoint(102.4, 82.5, -864);
    control[1] = new ControlPoint(91.5, 95.1, -926.9);
    control[2] = new ControlPoint(81.2, 94.4, -1071);
    control[3] = new ControlPoint(80.9, 90.1, -1155);

    curve = new Curve(nbCP, control);

    nbU = 100;

    curve->generateBSpline(nbU, 3);
    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    initPlanes();
}
