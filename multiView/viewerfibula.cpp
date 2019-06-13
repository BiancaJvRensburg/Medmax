#include "viewerfibula.h"

ViewerFibula::ViewerFibula(QWidget *parent, StandardCamera *camera) : Viewer (parent, camera)
{

}

void ViewerFibula::movePlanes(int position){

    /*int change = position - lastPos;

    leftPos->z += change;
    rightPos->z += change;

    lastPos = position;

    update();*/
}

Vec* ViewerFibula::initPosition(int side){
    double max = static_cast<double>(getMaxDistance());
    if(side==0) return new Vec(0,0,-max);
    else return new Vec(0,0,max);

    // TODO : calculate the actual return position
}

void ViewerFibula::createPlane(Vec side){
    float size = 10.0f;

    float x = static_cast<float>(side.x);
    float y = static_cast<float>(side.y);
    float z = static_cast<float>(side.z);

    glBegin(GL_QUADS);
        glVertex3f(x - size, y - size, z);
        glVertex3f(x - size, y + size, z);
        glVertex3f(x + size, y + size, z);
        glVertex3f(x + size, y - size, z);
    glEnd();
}

void ViewerFibula::moveLeftPlane(double percentage){
    /*double unitsMoved = percentage * 2*getMaxDistance();

    leftPos->z = - getMaxDistance() + unitsMoved;
    update();*/
}

void ViewerFibula::moveRightPlane(double percentage){
    /*double unitsMoved = percentage * 2*getMaxDistance();

    rightPos->z = getMaxDistance() - unitsMoved;
    update();*/
}

void ViewerFibula::setMaxDistance(double maxDistance){
    Viewer::setMaxDistance(200.0);
}
