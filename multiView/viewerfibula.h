#ifndef VIEWERFIBULA_H
#define VIEWERFIBULA_H

#include "viewer.h"

class ViewerFibula : public Viewer
{
    Q_OBJECT

public:
    ViewerFibula(QWidget *parent, StandardCamera *camera, int sliderMax, int fibulaOffsetMax);

public Q_SLOTS:
    void movePlanes(int);
    void moveLeftPlane(double);
    void moveRightPlane(double);

    void initCurve();

private:
    int indexOffset;
    int maxOffset;
};

#endif // VIEWERFIBULA_H
