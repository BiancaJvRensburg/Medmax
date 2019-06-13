#include "viewer.h"
#include "Triangle.h"
#include "Vec3D.h"
#include <QGLViewer/manipulatedFrame.h>

Viewer::Viewer(QWidget *parent, StandardCamera *cam) : QGLViewer(parent) {
    // Change the camera.
    Camera *c = camera();
    setCamera(cam);
    delete c;
}

void Viewer::draw() {
    glPushMatrix();   // Push the current modelView
    glMultMatrixd(manipulatedFrame()->matrix());  // Multiply the modelView by a manipulated frame
    drawAxis();

    glColor3f(1.,1.,1.);
    mesh.draw();

    glColor3f(1.0, 0, 0);
    leftPlane->draw();

    glColor3f(0, 1.0, 0);
    rightPlane->draw();

    curve->draw();
    curve->drawControl();

    glPopMatrix();    // Bring back the modelView
}

void Viewer::init() {
  // Restore previous viewer state.
  restoreStateFromFile();

  // Create the manipulated frame which will allow us to rotate the view
  setManipulatedFrame(new ManipulatedFrame());

  setAxisIsDrawn();

  zIncL = 0;
  setMaxDistance(50.0);

  mainAxis = Axis::X; // default

  // Set up the planes
  leftPlane = new Plane();
  rightPlane = new Plane();
  leftPlane->setSize(30.0);
  rightPlane->setSize(30.0);

  lastPosL = 0;
  lastPosR = 0;

  // The curve
  const long nbCP = 4;
  Point control[nbCP];

  startPoint = Point(-10, -10, 0);
  endPoint = Point(80, -10, 0);

  control[0] = startPoint;
  control[1] = Point(20, 20, 0);
  control[2] = Point(50, 20, 0);
  control[3] = endPoint;

  curve = new Curve(nbCP, control);

  nbU = 100;
  curve->generateBezierCasteljau(nbU);

  leftPlane->setPosition(startPoint.toVec());
  rightPlane->setPosition(endPoint.toVec());

  curveIndexR = nbU - 1;
  curveIndexL = 0;

  // Set up gl settings
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable (GL_POLYGON_OFFSET_LINE);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glLineWidth (1.0f);
}

QString Viewer::helpString() const {
  QString text("<h2>H e l p  B o x</h2>");
  text += "This is the help text";

  return text;
}

// TODO : block the slider

void Viewer::moveLeftPlane(int position){

    int change = position - lastPosL;

    if(change < 0 || curveIndexR > curveIndexL + change){   // Only move if we're going backwards or we haven't met the other plane
        curveIndexL += change;

        if(curveIndexL >= nbU) curveIndexL = nbU-1;
        else if(curveIndexL < 0) curveIndexL = 0;   // shouldn't ever happen

        leftPlane->setPosition(curve->getCurve()[curveIndexL].toVec());

        lastPosL = position;
        update();
        double percentage = position / (2.0 * maxDistance);
        Q_EMIT leftPosChanged(percentage);
    }
}

void Viewer::moveRightPlane(int position){

    int change = position - lastPosR;

    if(change < 0 || curveIndexR - change > curveIndexL){
        curveIndexR -= change;  // opposite to L because we start from the end

        if(curveIndexR >= nbU) curveIndexR = nbU-1;
        else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen

        rightPlane->setPosition(curve->getCurve()[curveIndexR].toVec());

        lastPosR = position;
        update();
        double percentage = position / (2.0 * maxDistance);
        Q_EMIT leftPosChanged(percentage);
    }
}

void Viewer::openOFF(QString filename) {
    std::vector<Vec3Df> &vertices = mesh.getVertices();
    std::vector<Triangle> &triangles = mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

    mainAxis = mesh.computeAxis();

    // Set the camera
    Vec3Df center;
    double radius;
    MeshTools::computeAveragePosAndRadius(vertices, center, radius);
    updateCamera(center, static_cast<float>(radius));
    update();
}

void Viewer::wheelEvent(QWheelEvent *e) {
  if ((camera()->type() == Camera::ORTHOGRAPHIC) && (((StandardCamera *)camera())->isStandard()) && (e->modifiers() & Qt::ShiftModifier)) {
    ((StandardCamera *)camera())->changeOrthoFrustumSize(e->delta());
    update();
  } else
    QGLViewer::wheelEvent(e);
}

Vec* Viewer::initPosition(int side){
    // TODO : change
    if(side==0) return new Vec(-maxDistance,0,-40);
    else return new Vec(maxDistance,0,-40);
}

double Viewer::getMaxDistance(){ return maxDistance; }
void Viewer::setMaxDistance(double mD){ maxDistance = mD; }

void Viewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(static_cast<double>(center[0]), static_cast<double>(center[1]), static_cast<double>(center[2])));
    camera()->setSceneRadius(static_cast<double>(radius*2.0f));
    camera()->showEntireScene();
}
