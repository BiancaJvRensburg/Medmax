#ifndef CURVEPOINT_H
#define CURVEPOINT_H

#include "controlpoint.h"

class CurvePoint : public ControlPoint
{
public:
    CurvePoint(ManipulatedFrame* mf);

    void draw();
};

#endif // CURVEPOINT_H
