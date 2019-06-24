#include "curve.h"
#include "math.h"
#include <GL/gl.h>

Curve::Curve(long nbCP)
{
    TabControlPoint = new ControlPoint*[nbCP];
    nbControlPoint = nbCP;

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = new ControlPoint();
    }

    initConnections();

}

Curve::Curve(long nbCP, Vec cntrlPoints[]){
    TabControlPoint = new ControlPoint*[nbCP];
    nbControlPoint = nbCP;

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = new ControlPoint(cntrlPoints[i].x, cntrlPoints[i].y, cntrlPoints[i].z);
    }

    initConnections();
}

void Curve::initConnections(){
    for(int i=0; i<nbControlPoint; i++){
        connect(TabControlPoint[i], &ControlPoint::cntrlPointTranslated, this, &Curve::reintialiseCurve);
    }
}

void Curve::generateBezierCasteljau(long n)
{
    curve = casteljau(TabControlPoint, nbControlPoint, n);
    dt = derivative();
    d2t = secondDerivative();
}

void Curve::reintialiseCurve(){
    generateBezierCasteljau(nbU);
}

Vec* Curve::casteljau(ControlPoint **TabControlPoint, long nbControlPoint, long n){
    nbU = n;
    Vec* c = new Vec[nbU];

    double x, y, z;
    for(int i=0; i<nbU; i++){
        double u = 1 / double(nbU-1) * i;
        //double coef = pow(1.0 - u, nbControlPoint);
        double coef = 1.0;
        Vec *p = finalPoint(TabControlPoint, nbControlPoint, u);
        x = p->x * coef;
        y = p->y * coef;
        z = p->z * coef;
        c[i] = Vec(x,y,z);
    }

    return c;
}

Vec* Curve::finalPoint(ControlPoint **TabControlPoint, long nbControlPoint, double u){
  if(nbControlPoint==1) return new Vec(TabControlPoint[0]->getX(), TabControlPoint[0]->getY(), TabControlPoint[0]->getZ());
  else{
    ControlPoint *newPoints[nbControlPoint-1];
    double x,y,z;

    for(int i=0; i<nbControlPoint-1; i++){
      x = (1-u) * TabControlPoint[i]->getX() + u * TabControlPoint[i+1]->getX();
      y = (1-u) * TabControlPoint[i]->getY() + u * TabControlPoint[i+1]->getY();
      z = (1-u) * TabControlPoint[i]->getZ() + u * TabControlPoint[i+1]->getZ();
      newPoints[i] = new ControlPoint(x,y,z);
    }
    return finalPoint(newPoints, nbControlPoint-1, u);
  }
}

void Curve::draw(){
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 1.0, 0.0);

      for(int i=0; i<nbU; i++){
        Vec p = curve[i];
        glVertex3f(p.x, p.y, p.z);
      }

      glEnd();
}

void Curve::drawControl(){
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 0.0, 1.0);

      for(int i=0; i<nbControlPoint; i++){
        Vec *p = new Vec(TabControlPoint[i]->getX(), TabControlPoint[i]->getY(), TabControlPoint[i]->getZ());
        glVertex3f(p->x, p->y, p->z);
      }

      glEnd();

      for(int i=0; i<nbControlPoint; i++){
          TabControlPoint[i]->draw();
      }
}

// Frenet frame
Vec* Curve::derivative(){
    const long nbCP = nbControlPoint - 1;

    if(nbCP<1) return NULL;

    ControlPoint *control[nbCP];

    for(int i=0; i<nbCP; i++){
        control[i] = new ControlPoint( (TabControlPoint[i+1]->getX() - TabControlPoint[i]->getX()) * nbCP,
                (TabControlPoint[i+1]->getY() - TabControlPoint[i]->getY()) * nbCP,
                (TabControlPoint[i+1]->getZ() - TabControlPoint[i]->getZ()) * nbCP);
    }

    return casteljau(control, nbCP, nbU);
}

Vec* Curve::secondDerivative(){
    const long nbCP = nbControlPoint - 2;

    if(nbCP<1) return NULL;

    ControlPoint *control[nbCP];

    for(int i=0; i<nbCP; i++){
        control[i] = new ControlPoint( (TabControlPoint[i+2]->getX() - 2.0 * TabControlPoint[i+1]->getX() + TabControlPoint[i]->getX()) * static_cast<double>(nbCP),
                (TabControlPoint[i+2]->getY() - 2.0 * TabControlPoint[i+1]->getY() + TabControlPoint[i]->getY()) * static_cast<double>(nbCP),
                (TabControlPoint[i+2]->getZ() - 2.0 * TabControlPoint[i+1]->getZ() + TabControlPoint[i]->getZ()) * static_cast<double>(nbCP));
    }

    return casteljau(control, nbCP, nbU);
}

Vec Curve::tangent(int index){
    Vec t = Vec(dt[index].x, dt[index].y, dt[index].z);
    t.normalize();

    return t;
}

Vec Curve::normal(int index){
    return cross(binormal(index), tangent(index));
}

Vec Curve::binormal(int index){
    Vec b = cross(Vec(dt[index].x, dt[index].y, dt[index].z), Vec(d2t[index].x, d2t[index].y, d2t[index].z));
    b.normalize();

    return b;
}

Vec Curve::orientation(int index){
    Vec t = tangent(index);
    Vec n = normal(index);
    Vec b = binormal(index);

    Vec newOrientation = n + t + b;

    return newOrientation;
}

void Curve::drawTangent(int index){
    Vec t = tangent(index);
    Vec n = normal(index);
    Vec b = binormal(index);

    glColor3f(0.0, 1.0, 1.0);
    glLineWidth(3);

    glBegin(GL_LINES);
      glVertex3f(curve[index].x, curve[index].y, curve[index].z);
      glVertex3f(curve[index].x + t.x*10, curve[index].y + t.y*10, curve[index].z + t.z*10);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINES);
      glVertex3f(curve[index].x, curve[index].y, curve[index].z);
      glVertex3f(curve[index].x + n.x*10, curve[index].y + n.y*10, curve[index].z + n.z*10);
    glEnd();

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
      glVertex3f(curve[index].x, curve[index].y, curve[index].z);
      glVertex3f(curve[index].x + b.x*10, curve[index].y + b.y*10, curve[index].z + b.z*10);
    glEnd();

   /* glColor3f(1.0, 0.0, 0.0);
    Vec newOrientation = orientation(index);
    glBegin(GL_LINES);
      glVertex3f(curve[index].x, curve[index].y, curve[index].z);
      glVertex3f(curve[index].x + newOrientation.x*10, curve[index].y + newOrientation.y*10, curve[index].z + newOrientation.z*10);
    glEnd();*/

    glLineWidth(1);
}
