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
        leftPlane->setPosition(curve->getCurve()[curveIndexL + indexOffset].toVec());
        rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset].toVec());
    }


    update();

}

void ViewerFibula::moveLeftPlane(double percentage){

    curveIndexL = static_cast<int>(percentage * static_cast<double>(nbU) );

    if(curveIndexL + indexOffset >= nbU) curveIndexL = nbU-1 - indexOffset;
    else if(curveIndexL + indexOffset < 0) curveIndexL = - indexOffset;

    leftPlane->setPosition(curve->getCurve()[curveIndexL + indexOffset].toVec());

    update();
}

void ViewerFibula::moveRightPlane(double percentage){

    curveIndexR = nbU - 1 - static_cast<int>(percentage * static_cast<double>(nbU) );

    if(curveIndexR >= nbU) curveIndexR = nbU-1;
    else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen

    rightPlane->setPosition(curve->getCurve()[curveIndexR + indexOffset].toVec());

    update();
}
