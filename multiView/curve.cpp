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

        // TODO : add size protection

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = cntrlPoints[i];
    }
}

void Curve::generateBezierCasteljau(long n)
{
    curve = casteljau(TabControlPoint, nbControlPoint, n);
    dt = derivative();
}

Point* Curve::casteljau(Point TabControlPoint[], long nbControlPoint, long n){
    nbU = n;
    Point* c = new Point[nbU];

    double x, y, z;
    for(int i=0; i<nbU; i++){
        double u = 1 / double(nbU-1) * i;
        //double coef = pow(1.0 - u, nbControlPoint);
        double coef = 1.0;
        Point * p = finalPoint(TabControlPoint, nbControlPoint, u);
        x = p->getX() * coef;
        y = p->getY() * coef;
        z = p->getZ() * coef;
        c[i] = Point(x,y,z);
    }

    return c;
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

void Curve::drawDerative(){
    Point * der = derivative();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 1.0, 1.0);

    for(int i=0; i<nbU; i++){
      Point p = der[i];
      glVertex3f(p.getX(), p.getY(), p.getZ());
    }

    glEnd();
}

// Frenet frame
Point* Curve::derivative(){
    const long nbCP = nbControlPoint -1;

    if(nbCP<1) return NULL;

    Point control[nbCP];

    for(int i=0; i<nbCP; i++){
        control[i] = Point( (TabControlPoint[i+1].getX() - TabControlPoint[i].getX()) * nbCP,
                (TabControlPoint[i+1].getY() - TabControlPoint[i].getY()) * nbCP,
                (TabControlPoint[i+1].getZ() - TabControlPoint[i].getZ()) * nbCP);
    }

    return casteljau(control, nbCP, nbU);
}

Vec Curve::tangent(int index){
    Vec t = Vec(dt[index].getX(), dt[index].getY(), dt[index].getZ());
    t.normalize();

    return t;
}

void Curve::drawTangent(int index){
    Vec t = tangent(index);

    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 1.0);
      glVertex3f(curve[index].getX(), curve[index].getY(), curve[index].getZ());
      glVertex3f(curve[index].getX() + t.x*10, curve[index].getY() + t.y*10, curve[index].getZ() + t.z*10);
    glEnd();
}

/*Curve* Curve::derivative(){
    const long nbCP = nbControlPoint -1;

    std::cout << "deriving" << std::endl;

    if(nbCP<1) return NULL;

    Point control[nbCP];

    for(int i=0; i<nbCP; i++){
        control[i] = Point( (TabControlPoint[i+1].getX() - TabControlPoint[i].getX()) * nbControlPoint,
                (TabControlPoint[i+1].getY() - TabControlPoint[i].getY()) * nbControlPoint,
                (TabControlPoint[i+1].getZ() - TabControlPoint[i].getZ()) * nbControlPoint);
    }

    for(int i=0; i<nbCP; i++){
        std::cout << control[i].getX() << " " << control->getY() << " " << control->getZ() << std::endl;
    }

    Curve* c = new Curve(nbCP, control);
    c->generateBezierCasteljau(nbU);

    std::cout << "generated" << std::endl;

    return new Curve();
}*/
