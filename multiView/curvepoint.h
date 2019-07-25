#ifndef CURVEPOINT_H
#define CURVEPOINT_H

#include "controlpoint.h"

class CurvePoint : public ControlPoint
{
    Q_OBJECT

public:
    CurvePoint(Vec* p, ManipulatedFrame* mf);

    void setPosition(Vec* p){ this->p->x = p->x;
                                this->p->y = p->y;
                                this->p->z = p->z; }

    void draw();

public Q_SLOTS:
    void cntrlMoved();

Q_SIGNALS:
    void curvePointTranslated(Vec offset);

private:
    ManipulatedFrame* manFrame;
    //double* curveIndex;
};

#endif // CURVEPOINT_H
