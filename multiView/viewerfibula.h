#ifndef VIEWERFIBULA_H
#define VIEWERFIBULA_H

#include "viewer.h"

class ViewerFibula : public Viewer
{
    Q_OBJECT

public:
    ViewerFibula(QWidget *parent, StandardCamera *camera);

public Q_SLOTS:
    void movePlanes(int);
    void moveLeftPlane(double);
    void moveRightPlane(double);

protected:
    Vec* initPosition(int side);
    void createPlane(Vec side);
    void setMaxDistance(double maxDistance);

private:
    int lastPos = 0;
};

#endif // VIEWERFIBULA_H
