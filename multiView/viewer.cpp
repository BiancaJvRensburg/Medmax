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

    this->nbU = new long();
    this->sliderMax = sliderMax;
    this->isDrawMesh = true;
}

void Viewer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glMultMatrixd(manipulatedFrame()->matrix());
    //drawAxis(20.0);

    glColor3f(1.,1.,1.);
    if(isDrawMesh) mesh.draw();

    //glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glColor3f(1.0, 0, 0);
    leftPlane->draw();

    glColor3f(0, 1.0, 0);
    rightPlane->draw();

    //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    curve->draw();

    curve->drawControl(); // We want to visualise this at all times
    // curve->drawTangent(curveIndexL);

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
  //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

  glLineWidth (1.0f);

}

QString Viewer::helpString() const {
  QString text("<h2>H e l p  B o x</h2>");
  text += "This is the help text";

  return text;
}

void Viewer::drawMesh(){
    if(isDrawMesh) isDrawMesh = false;
    else isDrawMesh = true;
    update();
}

void Viewer::cutMesh(){
    mesh.setIsCut(Side::INTERIOR, true);
    update();
}

void Viewer::uncutMesh(){
    mesh.setIsCut(Side::INTERIOR, false);
    update();
}

void Viewer::moveLeftPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = static_cast<int>(percentage * static_cast<double>(*nbU) );

    if(curveIndexR > index){   // Only move if we're going backwards or we haven't met the other plane
        curveIndexL = index;

        if(curveIndexL >= *nbU) curveIndexL = *nbU-1;
        else if(curveIndexL < 0) curveIndexL = 0;   // shouldn't ever happen
    }
    else if(curveIndexL == curveIndexR - 1) return;
    else curveIndexL = curveIndexR - 1;

    leftPlane->setPosition(curve->getPoint(curveIndexL), percentage);
    leftPlane->setOrientation(getNewOrientation(curveIndexL));

    mesh.updatePlaneIntersections(leftPlane);

    double distance = curve->discreteLength(curveIndexL, curveIndexR);

    update();
    Q_EMIT leftPosChanged(distance);

}

void Viewer::rotateLeftPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    leftPlane->rotatePlaneYZ(percentage);
    mesh.updatePlaneIntersections(leftPlane);
    update();
}

void Viewer::rotateRightPlane(int position){
    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);

    rightPlane->rotatePlaneYZ(percentage);
    mesh.updatePlaneIntersections(rightPlane);
    update();
}

void Viewer::moveRightPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = *nbU - 1 - static_cast<int>(percentage * static_cast<double>(*nbU) );

    if(index > curveIndexL){
        curveIndexR = index;

        if(curveIndexR >= *nbU) curveIndexR = *nbU-1;
        else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen
    }
    else if(curveIndexR == curveIndexL + 1) return;
    else curveIndexR = curveIndexL + 1;

    double percentageR = static_cast<double>(curveIndexR) / static_cast<double>(*nbU);

    rightPlane->setPosition(curve->getPoint(curveIndexR), percentageR);
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    mesh.updatePlaneIntersections(rightPlane);

    double distance = curve->discreteLength(curveIndexL, curveIndexR);

    update();
    Q_EMIT rightPosChanged(distance);
}

void Viewer::openOFF(QString filename) {
    std::vector<Vec3Df> &vertices = mesh.getVertices();
    std::vector<Triangle> &triangles = mesh.getTriangles();
    std::vector< std::vector<unsigned int>> &neighbours = mesh.getVertexNeighbours();
    std::vector< std::vector<unsigned int>> &vertexTriangles = mesh.getVertexTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles, neighbours, vertexTriangles);

    mesh.update();

    // Set the camera
    Vec3Df center;
    double radius;
    MeshTools::computeAveragePosAndRadius(vertices, center, radius);
    updateCamera(center, static_cast<float>(radius));

    update();
}

void Viewer::initCurve(){
    /*const long nbCP = 9;
    ControlPoint *control[nbCP];

    control[0] = new ControlPoint(-51.946, -19.1294, -18.4682);
    control[1] = new ControlPoint(-47.56, -22.74, -35.3152);
    control[2] = new ControlPoint(-38, -40, -60);
    control[3] = new ControlPoint(-24, -70, -70);

    control[4] = new ControlPoint(0, -91.5282, -74.3305);

    control[5] = new ControlPoint(24, -70, -70);
    control[6] = new ControlPoint(38, -40, -60);
    control[7] = new ControlPoint(47.56, -22.74, -35.3152);

    control[8] = new ControlPoint(51.946, -19.1294, -18.4682);*/

    const long nbCP = 9;
    ControlPoint *control[nbCP];

    control[0] = new ControlPoint(-53.3782, 6.81694, -5.29601);
    control[1] = new ControlPoint(-55.1869, -9.35275, -22.6458);
    control[2] = new ControlPoint(-45.0097, -35.0681, -50.9899);
    control[3] = new ControlPoint(-27.6007, -69.2743, -67.6769);

    control[4] = new ControlPoint(0, -91.5282, -74.3305);

    control[5] = new ControlPoint(27.6007, -69.2743, -67.6769);
    control[6] = new ControlPoint(45.0097, -35.0681, -50.9899);
    control[7] = new ControlPoint(55.1869, -9.35275, -22.6458);

    control[8] = new ControlPoint(53.3782, 6.81694, -5.29601);


    curve = new Curve(nbCP, control);

    connect(curve, &Curve::curveReinitialised, this, &Viewer::updatePlanes);

    *nbU = 200;
    curve->generateCatmull(nbU);

   initPlanes(Movable::STATIC);
}

void Viewer::initPlanes(Movable status){
    curveIndexR = *nbU - 1;
    curveIndexL = 0;

    leftPlane = new Plane(40.0, status);
    rightPlane = new Plane(40.0, status);


    leftPlane->setPosition(curve->getPoint(curveIndexL), 0);
    rightPlane->setPosition(curve->getPoint(curveIndexR), 1);

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));


    if(status == Movable::DYNAMIC){
        connect(leftPlane->cp, &CurvePoint::curvePointTranslated, curve, &Curve::moveToPoint);
        connect(rightPlane->cp, &CurvePoint::curvePointTranslated, curve, &Curve::moveToPoint);
    }

    mesh.addPlane(leftPlane);
    mesh.addPlane(rightPlane);
}

void Viewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(static_cast<double>(center[0]), static_cast<double>(center[1]), static_cast<double>(center[2])));
    camera()->setSceneRadius(static_cast<double>(radius*1.05f));
    camera()->showEntireScene();
}

void Viewer::updatePlanes(){
    double percentageL = static_cast<double>(curveIndexL) / static_cast<double>(*nbU);
    double percentageR = static_cast<double>(curveIndexR) / static_cast<double>(*nbU);

    leftPlane->setPosition(curve->getPoint(curveIndexL), percentageL);
    rightPlane->setPosition(curve->getPoint(curveIndexR), percentageR);

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    mesh.updatePlaneIntersections();

    update();
}

Quaternion Viewer::getNewOrientation(int index){
    /*Quaternion s;

    // Both planes need the same coordinate system
    s = leftPlane->fromRotatedBasis(curve->normal(index), curve->binormal(index), curve->tangent(index));

    return s.normalized();*/

    Quaternion s = Quaternion(Vec(0,0,1.0), curve->tangent(index));
    return s.normalized();
}


