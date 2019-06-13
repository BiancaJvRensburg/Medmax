#include "curve.h"
#include "math.h"
#include <GL/gl.h>

Curve::Curve(long nbCP)
{
    TabControlPoint = new Point[nbCP];
    nbControlPoint = nbCP;
}

Curve::Curve(long nbCP, Point cntrlPoints[]){
    TabControlPoint = new Point[nbCP];
    nbControlPoint = nbCP;
    nbCntrl = nbCP - 1;

        // TODO : add size protection

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = cntrlPoints[i];
    }
}

void Curve::generateBezierBernstein(long n)
{
    nbU = n;
    curve = new Point[nbU];

    Point sum;

  for(int j=0; j<nbU; j++){
    double u = 1 / double(nbU) * j;
    double sumx = 0.0;
    double sumy = 0.0;
    double sumz = 0.0;
   for(int i=0; i<=nbCntrl; i++){
      double coef = newton(double(i), double(nbCntrl)) * pow(u, i) * pow(1.0 - u, nbCntrl - i);
      sumx += coef * TabControlPoint[i].getX();
      sumy += coef * TabControlPoint[i].getY();
      sumz += coef * TabControlPoint[i].getZ();
    }
    curve[j] = Point(sumx, sumy, sumz);
  }
}

double Curve::newton(double i, double n)
{
  return factorial(n) / (factorial(i) * factorial(n-i));
}

double Curve::factorial(double n){
  if(n==0.0) return 1;
  else return n * factorial(n-1);
}

void Curve::generateBezierCasteljau(long n)
{
    nbU = n;
    curve = new Point[nbU];

  double x, y, z;
  for(int i=0; i<nbU; i++){
    double u = 1 / double(nbU-1) * i;
    //double coef = pow(1.0 - u, nbControlPoint);
    double coef = 1.0;
    Point * p = finalPoint(TabControlPoint, nbControlPoint, u);
    x = p->getX() * coef;
    y = p->getY() * coef;
    z = p->getZ() * coef;
    curve[i] = Point(x,y,z);
  }
}

Point* Curve::finalPoint(Point TabControlPoint[], long nbControlPoint, double u){
  if(nbControlPoint==1) return TabControlPoint;
  else{
    Point newPoints[nbControlPoint-1];
    double x,y,z;

    for(int i=0; i<nbControlPoint-1; i++){
      x = (1-u) * TabControlPoint[i].getX() + u * TabControlPoint[i+1].getX();
      y = (1-u) * TabControlPoint[i].getY() + u * TabControlPoint[i+1].getY();
      z = (1-u) * TabControlPoint[i].getZ() + u * TabControlPoint[i+1].getZ();
      newPoints[i] = Point(x,y,z);
    }
    return finalPoint(newPoints, nbControlPoint-1, u);
  }
}

void Curve::draw(){
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 1.0, 0.0);

      for(int i=0; i<nbU; i++){
        Point p = curve[i];
        glVertex3f(p.getX(), p.getY(), p.getZ());
      }

      glEnd();
}

void Curve::drawControl(){
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 0.0, 1.0);

      for(int i=0; i<nbControlPoint; i++){
        Point p = TabControlPoint[i];
        glVertex3f(p.getX(), p.getY(), p.getZ());
      }

      glEnd();
}
