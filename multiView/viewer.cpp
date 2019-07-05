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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glMultMatrixd(manipulatedFrame()->matrix());
    //drawAxis();

    glColor3f(1.,1.,1.);
    mesh.draw();

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);

    glColor3f(1.0, 0, 0);
    leftPlane->draw();

    glColor3f(0, 1.0, 0);
    rightPlane->draw();

    curve->draw();

    glDisable(GL_DEPTH);
    glDisable(GL_DEPTH_TEST);

    curve->drawControl(); // We want to visualise this at all times
    //curve->drawTangent(curveIndexL);

    glPopMatrix();
}

void Viewer::init() {
  setMouseTracking(true);

  restoreStateFromFile();

  setManipulatedFrame(new ManipulatedFrame());

  setAxisIsDrawn(false);

  initCurve();

  // Set up gl settings
  glEnable(GL_LIGHTING);

  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  //glEnable (GL_POLYGON_OFFSET_LINE);
  //glPolygonOffset (-1.0, 1.0);
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
    }
    else if(curveIndexL == curveIndexR - 1) return;
    else curveIndexL = curveIndexR - 1;

        leftPlane->setPosition(curve->getPoint(curveIndexL), percentage);
        leftPlane->setOrientation(getNewOrientation(curveIndexL));

        double distance = curve->discreteLength(curveIndexL, curveIndexR);

        update();
        Q_EMIT leftPosChanged(distance);

}

void Viewer::rotateLeftPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    leftPlane->rotatePlaneYZ(percentage);
    update();
}

void Viewer::rotateRightPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    rightPlane->rotatePlaneYZ(percentage);
    update();
}

void Viewer::moveRightPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = nbU - 1 - static_cast<int>(percentage * static_cast<double>(nbU) );

    if(index > curveIndexL){
        curveIndexR = index;

        if(curveIndexR >= nbU) curveIndexR = nbU-1;
        else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen
    }
    else if(curveIndexR == curveIndexL + 1) return;
    else curveIndexR = curveIndexL + 1;

        rightPlane->setPosition(curve->getPoint(curveIndexR), percentage);
        rightPlane->setOrientation(getNewOrientation(curveIndexR));

        double distance = curve->discreteLength(curveIndexL, curveIndexR);

        update();
        Q_EMIT rightPosChanged(distance);
}

void Viewer::openOFF(QString filename) {
    std::vector<Vec3Df> &vertices = mesh.getVertices();
    std::vector<Triangle> &triangles = mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

    mesh.update();

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

    control[0] = new ControlPoint(-45.7, -28.2, -36);
    control[1] = new ControlPoint(-20, -98.3, -90.1);
    control[2] = new ControlPoint(17, -104.4, -98.3);
    control[3] = new ControlPoint(37.2, -39, -41.5);

    int degree = 3;

    /*const long nbCP = 7;
    ControlPoint *control[nbCP];

    control[0] = new ControlPoint(10,0,0);
    control[1] = new ControlPoint(30, 40, 0);
    control[2] = new ControlPoint(70, 60, 0);
    control[3] = new ControlPoint(100, 80, 0);
    control[4] = new ControlPoint(130, 60, 0);
    control[5] = new ControlPoint(170, 40, 0);
    control[6] = new ControlPoint(200, 0, 0);

    int degree = 5;*/


    curve = new Curve(nbCP, control);

    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    nbU = 1000;
    // curve->generateBezierCasteljau(nbU);
    curve->generateBSpline(nbU, degree);

    //curve->addControlPoint(control[1]);

   initPlanes();
}

void Viewer::initPlanes(){
    curveIndexR = nbU - 1;
    curveIndexL = 0;

    leftPlane = new Plane(15.0);
    rightPlane = new Plane(15.0);


    leftPlane->setPosition(curve->getPoint(curveIndexL), 0);
    rightPlane->setPosition(curve->getPoint(curveIndexR), 1);

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    connect(leftPlane->cp, &CurvePoint::curvePointTranslated, curve, &Curve::moveToPoint);

}

void Viewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(static_cast<double>(center[0]), static_cast<double>(center[1]), static_cast<double>(center[2])));
    camera()->setSceneRadius(static_cast<double>(radius*1.05f));
    camera()->showEntireScene();
}

void Viewer::updatePlanes(){
    double percentageL = static_cast<double>(curveIndexL) / static_cast<double>(nbU);
    double percentageR = static_cast<double>(curveIndexR) / static_cast<double>(nbU);

    leftPlane->setPosition(curve->getPoint(curveIndexL), percentageL);
    rightPlane->setPosition(curve->getPoint(curveIndexR), percentageR);

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    update();
}

Quaternion Viewer::getNewOrientation(int index){
    Quaternion s;

    // Both planes need the same coordinate system
    s = leftPlane->fromRotatedBasis(curve->normal(index), curve->binormal(index), curve->tangent(index));

    return s.normalized();
}

double Viewer::angle(Vec a, Vec b){

    double na = a.normalize();
    double nb = b.normalize();

    double ab = a*b;

    return acos(ab / (na*nb));
}


