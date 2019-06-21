#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "point.h"
#include "camerapathplayer.h"
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

using namespace qglviewer;

class ControlPoint : public QObject
{

    Q_OBJECT

public:
    ControlPoint(){ p = new Point(0,0,0);}
    ControlPoint(Point* p);
    ControlPoint(double x, double y, double z);

    Point* getPoint(){ return p; }
    double getX(){ return p->getX(); }
    double getY(){ return p->getY(); }
    double getZ(){ return p->getZ(); }

    void initialise();
    void draw();

public Q_SLOTS:
    void cntrlMoved();

Q_SIGNALS:
    void cntrlPointTranslated();

private:
    Point* p;
    ManipulatedFrame* mf;
};

#endif // CONTROLPOINT_H
