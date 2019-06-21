#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "point.h"
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

using namespace qglviewer;

class CameraPathPlayer : public qglviewer::MouseGrabber {
public:
  CameraPathPlayer(int nb) : pathNb(nb) {}
  void checkIfGrabsMouse(int x, int y, const qglviewer::Camera *const camera);
  int yPos() { return 25 * pathNb; }

protected:
  void mousePressEvent(QMouseEvent *const, qglviewer::Camera *const camera) {
    camera->playPath(static_cast<unsigned int>(pathNb));
  }

private:
  int pathNb;
};

class ControlPoint
{
public:
    ControlPoint(){ p = new Point(0,0,0);}
    ControlPoint(Point* p);
    ControlPoint(double x, double y, double z);

    Point* getPoint(){ return p; }
    double getX(){ return p->getX(); }
    double getY(){ return p->getY(); }
    double getZ(){ return p->getZ(); }

    void draw();

private:
    Point* p;
    ManipulatedFrame mf;
};

#endif // CONTROLPOINT_H
