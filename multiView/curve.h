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
    Curve(long nbCP, ControlPoint *cntrlPoints[]);

    void generateBezierCasteljau(long nbU);
    void generateBSpline(long nbU, int degree);
    Vec** getCurve(){ return curve; }
    Vec* getPoint(int index){ return curve[index]; }

    Vec tangent(int index);
    Vec binormal(int index);
    Vec normal(int index);

    Vec orientation(int index);

    void draw();
    void drawControl();
    void drawTangent(int index);

public Q_SLOTS:
    void reintialiseCurve();

Q_SIGNALS:
    void curveReinitialised();

private:
    ControlPoint **TabControlPoint;
    long nbControlPoint;
    Vec **curve;
    long nbU;

    // BSpline
    int degree;
    double *knotVector;
    int knotIndex;

    Vec** casteljau(ControlPoint *TabControlPoint[], long nbControlPoint, long n);
    Vec* finalPoint(ControlPoint *TabControlPoint[], long nbControlPoint, double u);

    // BSpline
    void generateUniformKnotVector();
    Vec** basis(long nbU);
    Vec deBoor(double u, int i, int r);

    void initConnections();

    // Frenet frame
    Vec** derivative();
    Vec** dt;

    Vec** secondDerivative();
    Vec** d2t;

};

#endif // CURVE_H
