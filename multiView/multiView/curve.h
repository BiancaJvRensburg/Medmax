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

    //void generateBSpline(long nbU, int degree);
    Vec** getCurve(){ return curve; }
    Vec* getPoint(int index){ return curve[index]; }

    void generateCatmull(long nbU);

    Vec tangent(int index);
    Vec binormal(int index);
    Vec normal(int index);

    Vec orientation(int index);

    void draw();
    void drawControl();
    void drawTangent(int index);

    double discreteLength(int indexS, int indexE);      // Returns the discrete length between 2 points (One cut)
    int indexForLength(int indexS, double length);   // Returns the end index which will create a segment of a certain length

public Q_SLOTS:
    void reintialiseCurve();
    void addControlPoint(ControlPoint*);
    void moveToPoint(Vec, double);

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

    // BSpline
   /* double* generateUniformKnotVector(int k);
    Vec deBoor(double u, int i, int r);
    Vec** splineDerivative(int k);
    Vec deBoorDerivative(double u, int i, int r, int k);*/

    // Catmull rom
    void catmullrom();  // calculate the spline, first and second derivative
    void calculateCatmullPoints(Vec* c, Vec* cp, Vec* cpp, double t);

    double* generateCatmullKnotVector(double alpha);

    void initConnections();
    void updateConnections(ControlPoint*);

    // Frenet frame
    Vec** dt;
    Vec** d2t;

    void getModVec(int j, int r, double t, int kI, double offset, double* newPoints);

    int isSpace();  // Is there space for another control point?

    const int MAX_CNTRL_POINTS = 20;

};

#endif // CURVE_H
