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
    void addControlPoint(ControlPoint*);

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
    double* generateUniformKnotVector(int k);
    Vec deBoor(double u, int i, int r);
    Vec** splineDerivative(int k);
    Vec deBoorDerivative(double u, int i, int r, int k);

    void initConnections();
    void updateConnections(ControlPoint*);

    // Frenet frame
    Vec** derivative();
    Vec** dt;

    Vec** secondDerivative();
    Vec** d2t;

    double discreteLength(int indexS, int indexE);      // Returns the discrete length between 2 points (One cut)
    int indexForLength(int indexS, double length);   // Returns the end index which will create a segment of a certain length

    int isSpace();  // Is there space for another control point?

    const int MAX_CNTRL_POINTS = 20;

};

#endif // CURVE_H
