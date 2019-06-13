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
    //drawAxis();

    //glPolygonOffset (-1.0, 1.0);

    glColor3f(1.,1.,1.);
    mesh.draw();

    /*glColor3f(1.0, 0, 0);
    createPlane(*leftPos);

    glColor3f(0, 1.0, 0);
    createPlane(*rightPos);*/

    glColor3f(1.0, 0, 0);
    leftPlane->movePlane(new Vec(0,0,0), new Vec(1,1,1));

    /*glColor3f(0, 1.0, 0);
    rightPlane->movePlane(new Vec(0,0,0), new Vec(1,1,1));*/

    glPopMatrix();    // Bring back the modelView
}

void Viewer::init() {
  // Restore previous viewer state.
  restoreStateFromFile();

  // Create the manipulated frame which will allow us to rotate the view
  setManipulatedFrame(new ManipulatedFrame());

  //setAxisIsDrawn();

  zIncL = 0;
  setMaxDistance(50.0);

  //camera()->fitSphere(Vec(0,0,0), 100);

  mainAxis = Axis::X; // default

  // Open and read the mesh file
  //openOFF();

  /*leftPos = initPosition(0);
  rightPos = initPosition(1);*/

  // Set up the planes
  leftPlane = new Plane();
  rightPlane = new Plane();
  leftPlane->setSize(30.0);
  rightPlane->setSize(30.0);

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

/*void Viewer::createPlane(Vec side){
    float size = 20.0f;

    float x = static_cast<float>(side.x);
    float y = static_cast<float>(side.y);
    float z = static_cast<float>(side.z);

    switch (mainAxis) {
      case Axis::X:
        glBegin(GL_QUADS);
            glVertex3f(x, y - size, z - size);
            glVertex3f(x, y + size, z - size);
            glVertex3f(x, y + size, z + size);
            glVertex3f(x, y - size, z + size);
        glEnd();
        break;

      case Axis::Y:
        glBegin(GL_QUADS);
            glVertex3f(x - size, y, z - size);
            glVertex3f(x + size, y, z - size);
            glVertex3f(x + size, y, z + size);
            glVertex3f(x - size, y, z + size);
        glEnd();
        break;

      case Axis::Z:
        glBegin(GL_QUADS);
            glVertex3f(x - size, y - size, z);
            glVertex3f(x - size, y + size, z);
            glVertex3f(x + size, y + size, z);
            glVertex3f(x + size, y - size, z);
        glEnd();
        break;
    }
}*/

// TODO : block the slider

void Viewer::moveLeftPlane(int position){
    /*if(position <= 2*maxDistance - zIncR){
        zIncL = position;

        switch (mainAxis) {
          case Axis::X:
            leftPos->x = - maxDistance + zIncL;
            break;

          case Axis::Y:
            leftPos->y = - maxDistance + zIncL;
            break;

          case Axis::Z:
              leftPos->z = - maxDistance + zIncL;
              break;

        }
        update();
        double percentage = position / (2.0 * maxDistance);
        Q_EMIT leftPosChanged(percentage);
    }*/
}

void Viewer::moveRightPlane(int position){
    /*if(position <= 2*maxDistance - zIncL){
        zIncR = position;
        rightPos->x = maxDistance - zIncR;
        update();

        double percentage = position / (2.0 * maxDistance);
        Q_EMIT rightPosChanged(percentage);
    }*/
}

void Viewer::openOFF(QString filename) {

    //clear();

    std::vector<Vec3Df> &vertices = mesh.getVertices();
    std::vector<Triangle> &triangles = mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

    // mesh.Zero();
    mainAxis = mesh.computeAxis();

    // Set the camera
    //std::vector<Vec3Df> &vertices = mesh.getVertices();
    Vec3Df center;
    double radius;
    MeshTools::computeAveragePosAndRadius(vertices, center, radius);
    updateCamera(center, radius);

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
    if(side==0){

        return new Vec(-maxDistance,0,-40);
    }
    else{

        return new Vec(maxDistance,0,-40);
    }
}

double Viewer::getMaxDistance(){ return maxDistance; }
void Viewer::setMaxDistance(double mD){ maxDistance = mD; }

void Viewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(static_cast<double>(center[0]), static_cast<double>(center[1]), static_cast<double>(center[2])));
    camera()->setSceneRadius(static_cast<double>(radius*2.0f));
    camera()->showEntireScene();
}
