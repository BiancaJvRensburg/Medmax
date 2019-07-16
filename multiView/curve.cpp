#include "curve.h"
#include "math.h"
#include <GL/gl.h>

Curve::Curve(long nbCP)
{
    TabControlPoint = new ControlPoint*[static_cast<unsigned long long>(MAX_CNTRL_POINTS)];
    if(nbCP <= MAX_CNTRL_POINTS) nbControlPoint = nbCP;
    else nbControlPoint = MAX_CNTRL_POINTS;

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = new ControlPoint();
    }

    initConnections();
}

Curve::Curve(long nbCP, ControlPoint *cntrlPoints[]){  
    TabControlPoint = new ControlPoint*[static_cast<unsigned long long>(MAX_CNTRL_POINTS)];
    if(nbCP <= MAX_CNTRL_POINTS) nbControlPoint = nbCP;
    else nbControlPoint = MAX_CNTRL_POINTS;

    for(int i=0; i<nbCP; i++){
        TabControlPoint[i] = cntrlPoints[i];
    }

    initConnections();
}

void Curve::initConnections(){
    for(int i=0; i<nbControlPoint; i++){
        connect(TabControlPoint[i], &ControlPoint::cntrlPointTranslated, this, &Curve::reintialiseCurve);
    }
}

void Curve::updateConnections(ControlPoint* p){
    connect(p, &ControlPoint::cntrlPointTranslated, this, &Curve::reintialiseCurve);
}

void Curve::generateBSpline(long nbU, int degree){
    this->nbU = nbU;
    this->degree = degree;
    this->knotIndex = 0;

    this->knotVector = generateUniformKnotVector(0);
    curve = splineDerivative(0);
    dt =  splineDerivative(1);
    d2t = splineDerivative(2);
}

void Curve::generateCatmull(long nbU){
    /*this->nbU = nbU;
    this->degree = 3;
    this->knotIndex = 0;*/

    int nbSeg = nbControlPoint-3;

    this->nbU = nbU - nbU%nbSeg;
    //std::cout << "nbu :" << nbU << std::endl;
    this->knotIndex = 0;
    this->degree = 3;

    this->knotVector = generateCatmullKnotVector(0.5);
    curve = catmullrom(0);
    this->knotVector = generateUniformKnotVector(0);
    dt =  splineDerivative(1);
    d2t = splineDerivative(2);
}


Vec Curve::deBoor(double u, int j, int r){

    if(r==0) return Vec(TabControlPoint[j]->getX(), TabControlPoint[j]->getY(),TabControlPoint[j]->getZ());

    double alpha = (u - knotVector[j]) / (knotVector[j+degree-(r-1)] - knotVector[j]);

    return (1.0 - alpha) * deBoor(u, j-1, r-1) + alpha * deBoor(u, j, r-1);
}


// Returns the kth derivative of the curve ( 0 <= k <= 2 )
Vec** Curve::splineDerivative(int k){

    this->knotIndex = 0;

    Vec** c = new Vec*[static_cast<unsigned long long>(nbU)];

    for(int i=0; i<nbU; i++){
        double u = (1.0 / static_cast<double>(nbU-1)) * static_cast<double>(i);
        c[i] = new Vec();

        while(u >= knotVector[knotIndex+1] && knotVector[knotIndex+1] != 1.0) knotIndex++;

        *c[i] += Vec(deBoorDerivative(u, knotIndex, degree, k));
    }
    return c;
}

Vec Curve::deBoorDerivative(double u, int j, int r, int k){

    if(r > degree - k){
        double denom = static_cast<double>(degree + nbControlPoint + 1) - 2.0* static_cast<double>(degree) - 1.0;
        double rnorm = static_cast<double>(r) / denom;
        if(k==2){
                double beta = rnorm / (knotVector[j+degree-(r-1)] - knotVector[j]);
                return - beta * deBoorDerivative(u, j-1, r-1, k) + beta * deBoorDerivative(u, j, r-1, k);
            }
        else if(k==1){
            double beta = rnorm / knotVector[j+1] - knotVector[j];
            return beta * (deBoor(u, j, r-1) - deBoor(u, j-1, r-1));
        }
    }
    return deBoor(u, j, r);
}

double* Curve::generateUniformKnotVector(int a){
    int k = degree - a;
    int n = nbControlPoint - a;
    int m = n + k + 1;
    double* kv = new double[static_cast<unsigned long long>(m)];

    double denom = static_cast<double>(m) - 2.0* static_cast<double>(k) - 1.0;

    for(int i=0; i<=k; i++) kv[i] = 0;
    for(int i=k+1; i<m-k-1; i++) kv[i] = static_cast<double>(i-k) / denom;
    for(int i=m-k-1; i<m; i++) kv[i] = 1.0;

    return kv;
}

void Curve::moveToPoint(Vec offset, double t){
    int kIndex = 0;

    while(t >= knotVector[kIndex+1] && knotVector[kIndex+1] != 1.0) kIndex++;

    double* offsetPoints = new double[static_cast<unsigned long long>(degree+1)];
    for(int i=0; i<(degree+1); i++) offsetPoints[i] = 0;

    getModVec(kIndex, degree, t, kIndex, 1, offsetPoints);

    double maxOffset = 0;
    int maxJ = 0;

    for(int i=0; i<(degree+1); i++){
        int j = i + (kIndex - degree);
        if(offsetPoints[i] > maxOffset){
            maxOffset = offsetPoints[i];
            maxJ = j;
        }
    }

    // Fix the first and last points
    if(maxJ == 0){
        maxJ = 1;
        maxOffset = offsetPoints[1];
    }

    if(maxJ == nbControlPoint-1){
        maxJ = nbControlPoint-2;
        maxOffset = offsetPoints[degree-1];
    }

    offset *= 1.0 / maxOffset;

    TabControlPoint[maxJ]->moveControlPoint( Vec(TabControlPoint[maxJ]->getX() + offset.x, TabControlPoint[maxJ]->getY() + offset.y, TabControlPoint[maxJ]->getZ() + offset.z) );

    delete[] offsetPoints;
    reintialiseCurve();
}

void Curve::getModVec(int j, int r, double t, int kI, double offset, double* offsetPoints){
    if(r==0){
        offsetPoints[j - (kI-degree)] += offset;
        return;
    }
    double alpha = (t - knotVector[j]) / (knotVector[j + degree - (r-1)] - knotVector[j]);
    getModVec(j-1, r-1, t, kI, offset*(1.0 - alpha), offsetPoints);
    getModVec(j, r-1, t, kI, offset*alpha, offsetPoints);
}

void Curve::reintialiseCurve(){
    for(int i=0; i<nbU; i++){
        delete curve[i];
        delete dt[i];
        delete d2t[i];
    }

    this->knotVector = generateCatmullKnotVector(0.5);
    curve = catmullrom(0);
    this->knotVector = generateUniformKnotVector(0);
    dt =  splineDerivative(1);
    d2t = splineDerivative(2);

    Q_EMIT curveReinitialised();
}

void Curve::addControlPoint(ControlPoint* p){
    if(isSpace()==-1) return;

    int i = 0;
    ControlPoint* nextP;

    while(TabControlPoint[i]->getPoint() != p->getPoint()) i++;
    if(i < nbU-1) nextP = TabControlPoint[i+1];
    else return;    // don't do anything for the end point

    ControlPoint* halfway = new ControlPoint( (nextP->getX() + p->getX())/2.0, (nextP->getY() + p->getY())/2.0, (nextP->getZ() + p->getZ())/2.0);

    for(int j=nbControlPoint; j>i+1; j--) TabControlPoint[j] = TabControlPoint[j-1];
    TabControlPoint[i+1] = halfway;
    nbControlPoint++;
    generateBSpline(nbU, degree);
    updateConnections(halfway);
}

int Curve::isSpace(){
    if(nbControlPoint < MAX_CNTRL_POINTS) return 0;
    else return -1;
}

double* Curve::generateCatmullKnotVector(double alpha){
    double* kv = new double[static_cast<unsigned long long>(nbControlPoint)];

    kv[0] = 0;

    for(int i=1; i<nbControlPoint; i++){
        Vec p = *(TabControlPoint[i]->getPoint()) - *(TabControlPoint[i-1]->getPoint());
        kv[i] =  pow(p.norm(),alpha) + kv[i-1];
        //std::cout << i << " : " << kv[i] << std::endl;
    }

    //std::cout << "ok for the knot vector" << std::endl;

    return kv;
}

// Catmull rom
double* Curve::blendingFunction(double taux, double uVec[4]){
    double* result = new double[4];

    result[0] = -taux*uVec[1] + 2.0*taux*uVec[2] - taux*uVec[3];
    result[1] = uVec[0] + (taux-3.0)*uVec[2] + (2.0-taux)*uVec[3];
    result[2] = taux*uVec[2] + (3.0-2.0*taux)*uVec[2] + (taux-2.0)*uVec[3];
    result[3] = -taux*uVec[2] + taux*uVec[3];

    /*std::cout << "u : " << uVec[1];
    //for(int i=0; i<3; i++)
    std::cout << " with last coeff " << result[3] << " ";

    std::cout<< "\n" << std::endl;*/

    return result;
}

Vec* Curve::calculatePoint(double coeffs[4], ControlPoint* controls[4]){
    Vec* result = new Vec(0,0,0);

    for(int i=0; i<4; i++) *result += *controls[i]->getPoint()*coeffs[i];

    return result;
}

Vec* Curve::calculatePoint(double t){
    Vec* result = new Vec(0,0,0);

    Vec p[4] = {*TabControlPoint[knotIndex-1]->getPoint(), *TabControlPoint[knotIndex]->getPoint(), *TabControlPoint[knotIndex+1]->getPoint(), *TabControlPoint[knotIndex+2]->getPoint()};
   // Vec l0[3];

    //for(int i=0; i<4; i++)
    //std::cout << "p" << " : " << p[1].x <<" , " << p[1].y << " , " << p[1].z << std::endl;

    double t0 = knotVector[knotIndex-1];
    double t1 = knotVector[knotIndex];
    double t2 = knotVector[knotIndex+1];
    double t3 = knotVector[knotIndex+2];

    Vec a1 = (t1-t)/(t1-t0)*p[0] + (t-t0)/(t1-t0)*p[1];
    Vec a2 = (t2-t)/(t2-t1)*p[1] + (t-t1)/(t2-t1)*p[2];
    Vec a3 = (t3-t)/(t3-t2)*p[2] + (t-t2)/(t3-t2)*p[3];

    Vec b1 = (t2-t)/(t2-t0)*a1 + (t-t0)/(t2-t0)*a2;
    Vec b2 = (t3-t)/(t3-t1)*a2 + (t-t1)/(t3-t1)*a3;

    *result = (t2-t)/(t2-t1)*b1 + (t-t1)/(t2-t1)*b2;

    /*for(int i=knotIndex-1; i<knotIndex+2; i++){
        double alpha = (knotVector[i+1] - t) / (knotVector[i+1] - knotVector[i]);
        double beta = (t - knotVector[i]) / (knotVector[i+1] - knotVector[i]);
        l0[i - knotIndex -1] = alpha*p[i] + beta*p[i+1];
    }

    Vec l1[2];
    for(int i=knotIndex-1; i<knotIndex+1; i++){
        double alpha = (knotVector[i+2] - t) / (knotVector[i+2] - knotVector[i]);
        double beta = (t - knotVector[i]) / (knotVector[i+2] - knotVector[i]);
        l0[i - knotIndex -1] = alpha*p[i] + beta*p[i+1];
    }

    double alpha = (knotVector[knotIndex+1] - t) / (knotVector[knotIndex+1] - knotVector[knotIndex]);
    double beta = (t - knotVector[knotIndex]) / (knotVector[knotIndex+1] - knotVector[knotIndex]);*/

    //*result = alpha*l1[0] + beta*l1[1];

    //std::cout << " t : " << t << " point : " << result->x << " " << result->y << " " << result->z << std::endl;

    return result;
}

Vec** Curve::catmullrom(int k){
    int nbSeg = nbControlPoint-3;
    int uPerSeg = nbU/nbSeg;
    //nbU -= nbU%nbSeg;

    // std::cout << nbU << std::endl;
    //nbU = nbSeg/uPerSeg;

    //Vec** c = new Vec*[static_cast<unsigned long long>(nbU*nbSeg+1)];
    Vec** c = new Vec*[static_cast<unsigned long long>(nbU)];

    /*std::cout << "Nb segments : " << nbSeg << std::endl;
    std::cout << "Nb control : " << nbControlPoint << std::endl;

    // For each segment
    for(int j=1; j<=nbSeg; j++){
        std::cout << "and now?" << std::endl;
        // Find each u from 0 to 1
        for(int i=0; i<uPerSeg; i++){
            double u = (1.0 / static_cast<double>(uPerSeg-1)) * static_cast<double>(i);
            double uVec[4] = {1, u, u*u, u*u*u};
            if(k!=0){
                uVec[0] = 0;
                uVec[1] = 1;
                uVec[2] = 2.0*u;
                uVec[3] = 3.0*u*u;
            }
            ControlPoint* cps[4] = {TabControlPoint[j-1], TabControlPoint[j], TabControlPoint[j+1], TabControlPoint[j+2]};
            c[(j-1)*uPerSeg+i] = calculatePoint(blendingFunction(0.5, uVec), cps);

            std::cout<< "j : " << j << " and on curve " << (j-1)*uPerSeg+i <<  std::endl;
        }
    }*/

    knotIndex = 1;

    //std::cout << "Capacity : " << nbU*nbSeg+1 << std::endl;

    for(int j=1; j<=nbSeg; j++){
        // Find each u from 0 to 1
        int it=0;
        knotIndex = j;
        //std::cout << "\n\n knotIndex: " << knotIndex << std::endl;
        for(double i=knotVector[j]; i<knotVector[j+1]; i+=((knotVector[j+1]-knotVector[j])/static_cast<double>(uPerSeg))){
            //double u = (1.0 / static_cast<double>(nbU-1)) * static_cast<double>(i);
            c[(j-1)*uPerSeg+it] = new Vec();

            c[(j-1)*uPerSeg+it] = calculatePoint(i);
            //std::cout << "point " << (j-1)*nbU+it <<" recieved : " << c[(j-1)*nbU+it]->x << " , " << c[(j-1)*nbU+it]->y << " , " << c[(j-1)*nbU+it]->z << " , " << std::endl;
            it++;
        }
    }

    return c;
}

double Curve::discreteLength(int indexS, int indexE){
    return sqrt( pow((curve[indexE]->x - curve[indexS]->x), 2) + pow((curve[indexE]->y - curve[indexS]->y), 2) + pow((curve[indexE]->z - curve[indexS]->z), 2));
}

int Curve::indexForLength(int indexS, double length){
    int i=0;

    while(discreteLength(indexS, indexS+i) < length) i++;

    return indexS+i;
}

void Curve::draw(){
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);

      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 1.0, 0.0);

      for(int i=0; i<nbU; i++){
        Vec *p = curve[i];
        glVertex3f(static_cast<float>(p->x), static_cast<float>(p->y), static_cast<float>(p->z));
      }

      glEnd();

      glDisable(GL_DEPTH);
      glDisable(GL_DEPTH_TEST);
}

void Curve::drawControl(){
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0, 0.0, 1.0);

      for(int i=0; i<nbControlPoint; i++){
        Vec *p = new Vec(TabControlPoint[i]->getX(), TabControlPoint[i]->getY(), TabControlPoint[i]->getZ());
        glVertex3f(static_cast<float>(p->x), static_cast<float>(p->y), static_cast<float>(p->z));
      }

      glEnd();

      for(int i=0; i<nbControlPoint; i++){
          TabControlPoint[i]->draw();
      }
}

// Frenet frame

Vec Curve::tangent(int index){
    Vec t = Vec(dt[index]->x, dt[index]->y, dt[index]->z);
    t.normalize();

    return t;
}

Vec Curve::normal(int index){
    return cross(binormal(index), tangent(index));
}

Vec Curve::binormal(int index){
    Vec b = cross(Vec(dt[index]->x, dt[index]->y, dt[index]->z), Vec(d2t[index]->x, d2t[index]->y, d2t[index]->z));
    b.normalize();

    return b;
}

Vec Curve::orientation(int index){
    Vec t = tangent(index);
    Vec n = normal(index);
    Vec b = binormal(index);

    Vec newOrientation = n + t + b;

    return newOrientation;
}

void Curve::drawTangent(int index){
    Vec t = tangent(index);
    Vec n = normal(index);
    Vec b = binormal(index);

    glColor3f(0.0, 1.0, 1.0);
    glLineWidth(3);

    glBegin(GL_LINES);
      glVertex3f(static_cast<float>(curve[index]->x), static_cast<float>(curve[index]->y), static_cast<float>(curve[index]->z));
      glVertex3f(static_cast<float>(curve[index]->x + t.x*10), static_cast<float>(curve[index]->y + t.y*10), static_cast<float>(curve[index]->z + t.z*10));
    glEnd();

    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINES);
      glVertex3f(static_cast<float>(curve[index]->x), static_cast<float>(curve[index]->y), static_cast<float>(curve[index]->z));
      glVertex3f(static_cast<float>(curve[index]->x + n.x*10), static_cast<float>(curve[index]->y + n.y*10), static_cast<float>(curve[index]->z + n.z*10));
    glEnd();

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
      glVertex3f(static_cast<float>(curve[index]->x), static_cast<float>(curve[index]->y), static_cast<float>(curve[index]->z));
      glVertex3f(static_cast<float>(curve[index]->x + b.x*10), static_cast<float>(curve[index]->y + b.y*10), static_cast<float>(curve[index]->z + b.z*10));
    glEnd();

    glLineWidth(1);
}
