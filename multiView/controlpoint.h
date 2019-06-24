#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "camerapathplayer.h"
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

using namespace qglviewer;

class ControlPoint : public QObject
{

    Q_OBJECT

public:
    ControlPoint(){ p = new Vec(0,0,0);}
    ControlPoint(Vec* p);
    ControlPoint(double x, double y, double z);

    Vec* getPoint(){ return p; }
    double getX(){ return p->x; }
    double getY(){ return p->y; }
    double getZ(){ return p->z; }

    void initialise();
    void draw();

public Q_SLOTS:
    void cntrlMoved();

Q_SIGNALS:
    void cntrlPointTranslated();

private:
    Vec* p;
    ManipulatedFrame* mf;
};

#endif // CONTROLPOINT_H
