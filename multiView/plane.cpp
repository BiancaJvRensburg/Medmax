#include "plane.h"

Plane::Plane()
{
    position = new Vec(0, 0, 0);
    positionTranslation = new Vec(0, 0, 0);
    normal = new Vec(0, 0, 0);
    size = 1.0;

    initBasePlane();
}

Plane::Plane(Vec* pos, Vec* norm){
    movePosition(pos->x, pos->y, pos->z);

    initBasePlane();

    adjustNormal(norm->x, norm->y, norm->z);
}

void Plane::initBasePlane(){
    normalRotation = new Vec(0, 0, 0);

    points[0] = new Vec(position->x - size, position->y - size, position->z);
    points[1] = new Vec(position->x + size, position->y - size, position->z);
    points[2] = new Vec(position->x + size, position->y + size, position->z);
    points[3] = new Vec(position->x - size, position->y + size, position->z);
}

void Plane::movePosition(double x, double y, double z){
    positionTranslation->x = position->x - x;
    positionTranslation->y = position->y - y;
    positionTranslation->z = position->z - z;

    position->x = x;
    position->y = y;
    position->z = z;

    adjustPointsPosition();
}

void Plane::adjustNormal(double x, double y, double z){
    normalRotation->x = normal->x - x;
    normalRotation->y = normal->y - y;
    normalRotation->z = normal->z - z;

    normal->x = x;
    normal->y = y;
    normal->z = z;

    adjustPointsNormal();
}

void Plane::adjustPointsNormal(){

    for(int i=0; i<4; i++){
        points[i]->x += normalRotation->x;
        points[i]->y += normalRotation->y;
        points[i]->z += normalRotation->z;
    }
}

void Plane::adjustPointsPosition(){
    for(int i=0; i<4; i++){
        points[i]->x += positionTranslation->x;
        points[i]->y += positionTranslation->y;
        points[i]->z += positionTranslation->z;
    }
}

void Plane::draw(){
    glBegin(GL_QUADS);
    for(int i=0; i<3; i++)
        glVertex3f(points[i]->x, points[i]->y, points[i]->z);
    glEnd();
}

void Plane::movePlane(Vec* pos, Vec* norm){
    movePosition(pos->x, pos->y, pos->z);
    adjustNormal(norm->x, norm->y, norm->z);
    draw();
    /*for(int i=0; i<3; i++)
        std::cout << points[i]->x << "  " << points[i]->y << "  " << points[i]->z << std::endl;*/
}
