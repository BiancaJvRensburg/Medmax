#include "viewer.h"
#include "Triangle.h"
#include "Vec3D.h"
#include "controlpoint.h"
#include <QGLViewer/manipulatedFrame.h>

Viewer::Viewer(QWidget *parent, StandardCamera *cam, int sliderMax) : QGLViewer(parent) {
    // Change the camera.
    Camera *c = camera();
    setCamera(cam);
    delete c;

    this->sliderMax = sliderMax;
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
    //curve->drawTangent(curveIndexL);

    glPopMatrix();    // Bring back the modelView
}

void Viewer::init() {
    // For the mouse tracker
    setMouseTracking(true);

  // Restore previous viewer state.
  restoreStateFromFile();

  // Create the manipulated frame which will allow us to rotate the view
  setManipulatedFrame(new ManipulatedFrame());

  setAxisIsDrawn();

  initCurve();

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

void Viewer::moveLeftPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = static_cast<int>(percentage * static_cast<double>(nbU) );

    if(curveIndexR > index){   // Only move if we're going backwards or we haven't met the other plane
        curveIndexL = index;

        if(curveIndexL >= nbU) curveIndexL = nbU-1;
        else if(curveIndexL < 0) curveIndexL = 0;   // shouldn't ever happen

        leftPlane->setPosition(curve->getPoint(curveIndexL));
        leftPlane->setOrientation(getNewOrientation(curveIndexL));

        update();
        Q_EMIT leftPosChanged(percentage);
    }
}

void Viewer::rotateLeftPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    leftPlane->rotatePlaneZ(percentage);
    update();
}

void Viewer::rotateRightPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    rightPlane->rotatePlaneZ(percentage);
    update();
}

void Viewer::moveRightPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = nbU - 1 - static_cast<int>(percentage * static_cast<double>(nbU) );

    if(index > curveIndexL){
        curveIndexR = index;

        if(curveIndexR >= nbU) curveIndexR = nbU-1;
        else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen

        rightPlane->setPosition(curve->getPoint(curveIndexR));
        rightPlane->setOrientation(getNewOrientation(curveIndexR));

        update();
        Q_EMIT rightPosChanged(percentage);
    }
}

void Viewer::openOFF(QString filename) {
    std::vector<Vec3Df> &vertices = mesh.getVertices();
    std::vector<Triangle> &triangles = mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

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

void Viewer::initCurve(){
    const long nbCP = 4;
    ControlPoint *control[nbCP];

    ControlPoint *startPoint = new ControlPoint(-50, -30, -30);
    ControlPoint *endPoint = new ControlPoint(50, -30, -30);

    control[0] = startPoint;
    control[1] = new ControlPoint(-30, -120, -100);
    control[2] = new ControlPoint(20, -120, -100);
    control[3] = endPoint;

    curve = new Curve(nbCP, control);

    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    nbU = 100;
    curve->generateBezierCasteljau(nbU);

    initPlanes();
}

void Viewer::initPlanes(){
    curveIndexR = nbU - 1;
    curveIndexL = 0;

    leftPlane = new Plane(10.0);
    rightPlane = new Plane(10.0);

    leftPlane->setPosition(curve->getPoint(curveIndexL));
    rightPlane->setPosition(curve->getPoint(curveIndexR));

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    /*Vec yAxis = Vec(0,1,0);
    Vec t = curve->tangent(curveIndexL);

    double theta = angle(t, yAxis);

    leftPlane->rotatePlane(Vec(0,0,1), theta);*/

}

void Viewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(static_cast<double>(center[0]), static_cast<double>(center[1]), static_cast<double>(center[2])));
    camera()->setSceneRadius(static_cast<double>(radius*1.05f));
    camera()->showEntireScene();
}

void Viewer::updatePlanes(){
    leftPlane->setPosition(curve->getPoint(curveIndexL));
    rightPlane->setPosition(curve->getPoint(curveIndexR));

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    update();
}

Quaternion Viewer::getNewOrientation(int index){
    Quaternion s;

    Vec norm = curve->normal(index);

    s = Quaternion(Vec(0,0,1.0), norm);
    return s.normalized();
}

double Viewer::angle(Vec a, Vec b){
    Vec ab = Vec(a.x*b.x, a.y*b.y, a.z*b.z);
    double na = a.normalize();
    double nb = b.normalize();
    double nab = ab.normalize();

    return acos(nab / (na*nb));
}


