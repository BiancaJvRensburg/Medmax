#ifndef CURVE_H
#define CURVE_H

#include "point.h"
#include <QGLViewer/qglviewer.h>

using namespace qglviewer;

class Curve
{
public:
    Curve(){}
    Curve(long nbCP);
    Curve(long nbCP, Point cntrlPoints[]);

    void generateBezierCasteljau(long nbU);
    Point* getCurve(){ return curve; }

    Vec tangent(int index);
    Vec binormal(int index);
    Vec normal(int index);

    Vec orientation(int index);

    void draw();
    void drawControl();
    void drawTangent(int index);

private:
    Point* TabControlPoint;
    long nbControlPoint;
    Point* curve;
    long nbU;

    Point* casteljau(Point TabControlPoint[], long nbControlPoint, long n);
    Point* finalPoint(Point TabControlPoint[], long nbControlPoint, double u);

    // Frenet frame
    Point* derivative();
    Point* dt;

    Point* secondDerivative();
    Point* d2t;

};

#endif // CURVE_H
