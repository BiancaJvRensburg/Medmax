#ifndef CURVE_H
#define CURVE_H

#include "point.h"

class Curve
{
public:
    Curve(){}
    Curve(long nbCP);
    Curve(long nbCP, Point cntrlPoints[]);

    /*void setControlPoints(Point pInit);
    void setControlPoints(int nbPoints, Point* tabControl);
    void addControlPoint(Point p);*/

    void generateBezierBernstein(long nbU);
    void generateBezierCasteljau(long nbU);
    Point* getCurve(){ return curve; }

    void draw();
    void drawControl();

private:
    Point* TabControlPoint;
    long nbControlPoint;    // the actual size of the control table
    long nbCntrl;       // the number we'll use
    Point* curve;
    long nbU;

    double newton(double i, double n);
    double factorial(double n);
    Point* finalPoint(Point TabControlPoint[], long nbControlPoint, double u);
};

#endif // CURVE_H
