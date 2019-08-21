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
    this->isDrawMesh = false;
    this->nbGhostPlanes = 3;
    this->isGhostPlanes = false;
}

void Viewer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glMultMatrixd(manipulatedFrame()->matrix());
    //drawAxis(20.0);

    glColor3f(1.,1.,1.);
    mesh.draw();
    if(isDrawMesh) mesh.drawCut();

    if(isGhostPlanes) drawPolyline();

    //glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glColor3f(1.0, 0, 0);
    leftPlane->draw();

    glColor3f(0, 1.0, 0);
    rightPlane->draw();

    glColor3f(0,0,0);
    for(unsigned int i=0; i<ghostPlanes.size(); i++) ghostPlanes[i].draw();

    //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    curve->draw();

    //curve->drawControl(); // We want to visualise this at all times
    // curve->drawTangent(curveIndexL);

    glPopMatrix();
}

void Viewer::updatePolyline(){
    polyline.clear();

    Vec p = *leftPlane->getPosition();
    polyline.push_back(p);

    for(unsigned int i=0; i<ghostPlanes.size(); i++){
        if(ghostPlanes[i].getCurvePoint()==NULL) continue;
        p = Vec(ghostPlanes[i].getCurvePoint()->getX(), ghostPlanes[i].getCurvePoint()->getY(), ghostPlanes[i].getCurvePoint()->getZ());
        polyline.push_back(p);
    }

    p = *rightPlane->getPosition();
    polyline.push_back(p);

    getPolylinePlaneAngles();
}

void Viewer::drawPolyline(){
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glColor3f(0.0, 1.0, 0.0);

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 0.0, 1.0);

        for(unsigned int i=0; i<polyline.size(); i++){
            glVertex3f(static_cast<float>(polyline[i].x), static_cast<float>(polyline[i].y), static_cast<float>(polyline[i].z));
        }

    glEnd();

    glDisable(GL_DEPTH);
    glDisable(GL_DEPTH_TEST);
}

void Viewer::init() {
  setMouseTracking(true);

  restoreStateFromFile();

  viewerFrame = new ManipulatedFrame();

  setManipulatedFrame(viewerFrame);

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

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

int Viewer::partition(int sorted[], int start, int end){
    int p = sorted[end];
    int index = start - 1;

    for(int i=start; i<end; i++){
        double tangentAngleA = angle(curve->tangent(sorted[i]-1), curve->tangent(sorted[i]));
        double tangentAngleP = angle(curve->tangent(p-1), curve->tangent(p));

        if(tangentAngleA >= tangentAngleP){
            index++;
            swap(&sorted[index], &sorted[i]);
        }
    }
    swap(&sorted[index+1], &sorted[end]);
    return index+1;
}

void Viewer::quicksort(int sorted[], int start, int end){
    if(start < end){
        int p = partition(sorted, start, end);
        quicksort(sorted, start, p-1);
        quicksort(sorted, p+1, end);
    }
}

void Viewer::drawMesh(){
    if(isDrawMesh) isDrawMesh = false;
    else isDrawMesh = true;
    update();
}

void Viewer::initGhostPlanes(){
    ghostPlanes.clear();
    //delete[] ghostLocation;

    int finalNb = nbGhostPlanes;

    int maxIndicies[nbGhostPlanes];

    const int startI = curve->indexForLength(curveIndexL, constraint);
    const int endI = curve->indexForLength(curveIndexR, -constraint);
    const int searchArea = endI - startI;

    if(searchArea > 0){

        int sorted[searchArea];

        for(int i=0; i<searchArea; i++){
            sorted[i] = startI+i;
        }

        quicksort(sorted, 0, searchArea-1);

        maxIndicies[0] = sorted[0];
        int sortedIndex = 1;

        for(int i=1; i<nbGhostPlanes; i++){
            // the constraint
            bool tooClose;
            do{
                tooClose = false;
                for(int j=i-1; j>=0; j--){
                    if(sortedIndex < searchArea && curve->discreteLength(maxIndicies[j],sorted[sortedIndex])<constraint){
                        tooClose = true;
                        break;
                    }
                }
                if(tooClose) sortedIndex++;
            }while(tooClose);

            if(sortedIndex >= searchArea){
                finalNb = i;
                break;
            }
            maxIndicies[i] = sorted[sortedIndex];
            sortedIndex++;  // move with i
        }

        // sort the planes
        for(int i=0; i<finalNb; i++){
            for(int j=i+1; j<finalNb; j++){
                if(maxIndicies[i] > maxIndicies[j]) swap(&maxIndicies[i], &maxIndicies[j]);
            }
        }

       // ghostLocation = new int[finalNb];

        ghostLocation.clear();
        for(int i=0; i<finalNb; i++) ghostLocation.push_back(maxIndicies[i]);

        currentNbGhostPlanes = finalNb;

        addGhostPlanes(finalNb);

        for(unsigned int i=0; i<ghostPlanes.size(); i++) connect(ghostPlanes[i].getCurvePoint(), &CurvePoint::curvePointTranslated, this, &Viewer::ghostPlaneMoved);

        // Update the fibula planes
        double distance = curve->discreteLength(curveIndexL, ghostLocation[0]);
        Q_EMIT leftPosChanged(distance);
        distance = curve->discreteLength(curveIndexR, ghostLocation[finalNb-1]);
        Q_EMIT rightPosChanged(distance);

    }
    else Q_EMIT ghostPlanesAdded(0,0);

    updatePolyline();
}

void Viewer::cutMesh(){
    mesh.setIsCut(Side::INTERIOR, true);
    isGhostPlanes = true;
    initGhostPlanes();
    update();
}

void Viewer::uncutMesh(){
    mesh.setIsCut(Side::INTERIOR, false);
    isGhostPlanes = false;
    ghostPlanes.clear();
    update();
}

void Viewer::moveLeftPlane(int position){

    double percentage = static_cast<double>(position) / static_cast<double>(sliderMax);
    int index = static_cast<int>(percentage * static_cast<double>(*nbU) );

    if( (curve->indexForLength(curveIndexR, -constraint) > index)){  // Only move if we're going backwards or we haven't met the other plane
        curveIndexL = index;

        if(curveIndexL >= *nbU) curveIndexL = *nbU-1;
        else if(curveIndexL < 0) curveIndexL = 0;   // shouldn't ever happen
    }
    else if( (curveIndexL == curve->indexForLength(curveIndexR, -constraint)) ) return;
    else curveIndexL = curve->indexForLength(curveIndexR, -constraint);

    if(isGhostPlanes) initGhostPlanes();

    leftPlane->setPosition(curve->getPoint(curveIndexL));
    leftPlane->setOrientation(getNewOrientation(curveIndexL));

    mesh.updatePlaneIntersections(leftPlane);

    double distance;

    if(!isGhostPlanes) distance = curve->discreteChordLength(curveIndexL, curveIndexR);
    else{
        updatePolyline();
        distance = curve->discreteLength(curveIndexL, ghostLocation[0]);
    }

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

    if( index > curve->indexForLength(curveIndexL, constraint)){
        curveIndexR = index;

        if(curveIndexR >= *nbU) curveIndexR = *nbU-1; // shouldn't ever happen either, outside of testing
        else if(curveIndexR < 0) curveIndexR = 0;   // shouldn't ever happen
    }
    else if(curveIndexR == curve->indexForLength(curveIndexL, constraint)) return;
    else curveIndexR = curve->indexForLength(curveIndexL, constraint);

    if(isGhostPlanes) initGhostPlanes();

    rightPlane->setPosition(curve->getPoint(curveIndexR));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    mesh.updatePlaneIntersections(rightPlane);

    double distance;

    if(!isGhostPlanes) distance = curve->discreteChordLength(curveIndexL, curveIndexR);
    else{
        updatePolyline();
        distance = curve->discreteLength(ghostLocation[ghostPlanes.size()-1], curveIndexR);
    }

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

    control[0] = new ControlPoint(-56.9335, -13.9973, 8.25454);

    control[1] = new ControlPoint(-50.8191, -20.195, -19.53);
    control[2] = new ControlPoint(-40.155, -34.5957, -50.7005);
    control[3] = new ControlPoint(-27.6007, -69.2743, -67.6769);

    control[4] = new ControlPoint(0, -85.966, -68.3154);

    control[5] = new ControlPoint(26.7572, -69.0705, -65.6261);
    control[6] = new ControlPoint(40.3576, -34.3609, -50.7634);
    control[7] = new ControlPoint(46.2189, -21.3245, -17.9009);

    control[8] = new ControlPoint(52.3669, -15.4613, 8.70223);


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


    leftPlane->setPosition(curve->getPoint(curveIndexL));
    rightPlane->setPosition(curve->getPoint(curveIndexR));

    leftPlane->setOrientation(getNewOrientation(curveIndexL));
    rightPlane->setOrientation(getNewOrientation(curveIndexR));

    mesh.addPlane(leftPlane);
    mesh.addPlane(rightPlane);
}

void Viewer::addGhostPlanes(int nb){
    double distances[nb+1];     // +1 for the last plane

    for(unsigned int i=0; i<static_cast<unsigned int>(nb); i++){
        //std::cout << "adding plane " << this << std::endl;
        ghostPlanes.push_back(Plane(40.0, Movable::DYNAMIC));
        //std::cout << "plane added " << this << std::endl;
        ghostPlanes[i].setOrientation(getNewOrientation(ghostLocation[i]));
        ghostPlanes[i].setPosition(curve->getPoint(ghostLocation[i]));
        if(i==0) distances[i] = curve->discreteLength(curveIndexL, ghostLocation[i]);
        else distances[i] = curve->discreteLength(ghostLocation[i-1], ghostLocation[i]);
    }

    distances[nb] = curve->discreteLength(ghostLocation[nb-1], curveIndexR);

    Q_EMIT ghostPlanesAdded(nb, distances);
}

void Viewer::ghostPlaneMoved(){
    // get the new distances
    // Optimisation: just get the distances from the plane moved

    //std::cout << "ghost planes moved";

    unsigned int nb = ghostPlanes.size();
    double distances[nb+1];     // +1 for the last plane

    for(unsigned int i=0; i<static_cast<unsigned int>(nb); i++){
        if(i==0) distances[i] = segmentLength(*(leftPlane->getPosition()), *(ghostPlanes[i].getCurvePoint()->getPoint()));
        else distances[i] = segmentLength(*(ghostPlanes[i-1].getCurvePoint()->getPoint()), *(ghostPlanes[i].getCurvePoint()->getPoint()));
    }

    distances[nb] = segmentLength(*(rightPlane->getPosition()), *(ghostPlanes[nb-1].getCurvePoint()->getPoint()));

    Q_EMIT ghostPlanesTranslated(nb, distances);

    updatePolyline();
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

    mesh.updatePlaneIntersections();

    update();
}

Quaternion Viewer::getNewOrientation(int index){
    Quaternion s = Quaternion(Vec(0,0,1.0), curve->tangent(index));
    return s.normalized();
}

void Viewer::getPolylinePlaneAngles(){
    // There are always 2 intersections for each polyline segment
    if(polyline.size()==0) return;
    double angles[2*(polyline.size()-1)];

    angles[0] = leftPlane->getIntersectionAngle(polyline[1]);

    for(int i=1; i<polyline.size()-1; i++){
        angles[2*i-1] = ghostPlanes[i-1].getIntersectionAngle(polyline[i-1]);
        angles[2*i] = ghostPlanes[i-1].getIntersectionAngle(polyline[i+1]);
    }

    angles[2*(polyline.size()-1)-1] = rightPlane->getIntersectionAngle(polyline[polyline.size()-2]);

    //for(int i=0; i<2*(polyline.size()-1); i++) std::cout << i << " : " << angles[i] * 180.0 / M_PI << std::endl;
}

double Viewer::angle(Vec a, Vec b){

    double na = a.norm();
    double nb = b.norm();

    double ab = a*b;

    return acos(ab / (na*nb));
}

double Viewer::segmentLength(Vec a, Vec b){
    return sqrt( pow((b.x - a.x), 2) + pow((b.y - a.y), 2) + pow((b.z - a.z), 2));
}


