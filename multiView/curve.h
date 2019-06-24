#ifndef CURVE_H
#define CURVE_H

#include <QGLViewer/qglviewer.h>
#include "controlpoint.h"

using namespace qglviewer;

class Curve : public QObject
{
    Q_OBJECT

public:
    Curve(){}
    Curve(long nbCP);
    Curve(long nbCP, Vec cntrlPoints[]);

    void generateBezierCasteljau(long nbU);
    Vec* getCurve(){ return curve; }

    Vec tangent(int index);
    Vec binormal(int index);
    Vec normal(int index);

    Vec orientation(int index);

    void draw();
    void drawControl();
    void drawTangent(int index);

public Q_SLOTS:
    void reintialiseCurve();

private:
    ControlPoint **TabControlPoint;
    long nbControlPoint;
    Vec* curve;
    long nbU;

    Vec* casteljau(ControlPoint *TabControlPoint[], long nbControlPoint, long n);
    Vec* finalPoint(ControlPoint *TabControlPoint[], long nbControlPoint, double u);

    void initConnections();

    // Frenet frame
    Vec* derivative();
    Vec* dt;

    Vec* secondDerivative();
    Vec* d2t;

};

#endif // CURVE_H
