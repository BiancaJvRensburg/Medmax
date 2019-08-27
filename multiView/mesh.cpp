#include "mesh.h"
#include <algorithm>
#include <float.h>

void Mesh::computeBB(){

    BBMin = Vec3Df( FLT_MAX, FLT_MAX, FLT_MAX );
    BBMax = Vec3Df( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( unsigned int i = 0 ; i < vertices.size() ; i ++ ){
        const Vec3Df & point = vertices[i];
        for( int v = 0 ; v < 3 ; v++ ){
            float value = point[v];
            if( BBMin[v] > value ) BBMin[v] = value;
            if( BBMax[v] < value ) BBMax[v] = value;
        }
    }

    radius = (BBMax - BBMin).norm();

    BBCentre = (BBMax + BBMin)/2.0f;
}

void Mesh::update(){
    computeBB();
    recomputeNormals();
    updatePlaneIntersections();
}

void Mesh::clear(){
    vertices.clear();

    triangles.clear();

    normals.clear();
    verticesNormals.clear();

}

void Mesh::recomputeNormals () {

    computeTriangleNormals();
    computeVerticesNormals();

}

void Mesh::computeTriangleNormals(){

    normals.clear();

    for(unsigned int i = 0 ; i < triangles.size() ; i++){
        normals.push_back(computeTriangleNormal(i));
    }

}

Vec3Df Mesh::computeTriangleNormal(unsigned int id ){

    const Triangle & t = triangles[id];
    Vec3Df normal = Vec3Df::crossProduct(vertices[t.getVertex (1)] - vertices[t.getVertex (0)], vertices[t.getVertex (2)]- vertices[t.getVertex (0)]);
    normal.normalize();
    return normal;

}

void Mesh::setIsCut(Side s, bool isCut){
    this->isCut = isCut;
    this->cuttingSide = s;
    if(isCut) updatePlaneIntersections();
}

void Mesh::computeVerticesNormals(){

    verticesNormals.clear();
    verticesNormals.resize( vertices.size() , Vec3Df(0.,0.,0.) );

    for( unsigned int t = 0 ; t < triangles.size(); ++t )
    {
        Vec3Df const & tri_normal = normals[t];

        verticesNormals[ triangles[t].getVertex(0) ] += tri_normal;
        verticesNormals[ triangles[t].getVertex(1) ] += tri_normal;
        verticesNormals[ triangles[t].getVertex(2) ] += tri_normal;
    }

    for( unsigned int v = 0 ; v < verticesNormals.size() ; ++v )
    {
        verticesNormals[ v ].normalize();
    }
}

// Access and colour each individual vertex here
void Mesh::glTriangle(unsigned int i){
    const Triangle & t = triangles[i];

    for(unsigned int j = 0 ; j < 3 ; j++ ){
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(vertices[t.getVertex(j)]);
    }

    glColor3f(1.0, 1.0, 1.0);
}

void Mesh::glTriangleSmooth(unsigned int i){
    const Triangle & t = triangles[i];

    for(unsigned int j = 0 ; j < 3 ; j++ ){
        if(cuttingSide == Side::EXTERIOR){
            /*if(flooding[t.getVertex(j)] == 0) glColor3f(0, 0, 1);
            if(flooding[t.getVertex(j)] == 1) glColor3f(0, 0.5, 0.5);
            if(flooding[t.getVertex(j)] == planes.size()) glColor3f(1, 0, 0);
            if(flooding[t.getVertex(j)] == planes.size()+1) glColor3f(1, 1, 0);*/
            /*if(flooding[t.getVertex(j)] == 2) glColor3f(0, 0, 1);
            if(flooding[t.getVertex(j)] == 3) glColor3f(0, 0.5, 0.5);
            if(flooding[t.getVertex(j)] == planes.size()+2) glColor3f(1, 0, 0);
            if(flooding[t.getVertex(j)] == planes.size()+3) glColor3f(1, 1, 0);*/
            for(int i=0; i<segmentsConserved.size(); i++){
                if(flooding[t.getVertex(j)] == segmentsConserved[i]) glColor3f(0, 0, 1);
            }
        }

        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(smoothedVerticies[t.getVertex(j)]);
    }

    glColor3f(1.0, 1.0, 1.0);
}

void Mesh::glTriangleFibInMand(unsigned int i){
    const Triangle & t = fibInMandTriangles[i];

    glColor3f(1.0, 0, 0);

    //  TODO : normals are all wrong

    for(unsigned int j = 0 ; j < 3 ; j++ ){
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(fibInMandVerticies[t.getVertex(j)]);
    }

    glColor3f(1.0, 1.0, 1.0);
}

void Mesh::addPlane(Plane *p){
    planes.push_back(p);
    planeNeighbours.push_back(-1);  // This is done once for the neg and once for the pos
    planeNeighbours.push_back(-1);
    std::vector<unsigned int> init;
    intersectionTriangles.push_back(init);
    updatePlaneIntersections(p);
    //std::cout << "Plane size : " << planeNeighbours.size() << std::endl;
}

void Mesh::updatePlaneIntersections(){
    if(isCut){
        flooding.clear();
        for(int i=0; i<vertices.size(); i++) flooding.push_back(-1);
        for(int i=0; i<planeNeighbours.size(); i++) planeNeighbours[i] = -1;

        for(unsigned int i=0; i<planes.size(); i++) planeIntersection(i);

        for(unsigned int i=0; i<flooding.size(); i++){
            if(flooding[i] != -1){
                for(unsigned int j=0; j<vertexNeighbours[i].size(); j++){
                floodNeighbour(vertexNeighbours[i][j], flooding[i]);
                }
            }
        }

        mergeFlood();
        cutMesh();
    }
}

void Mesh::cutMesh(){
    trianglesCut.clear();

    bool truthTriangles[triangles.size()];  // keeps a record of the triangles who are already added
    for(unsigned int i=0; i<triangles.size(); i++) truthTriangles[i] = false;

    switch (cuttingSide) {
        case Side::INTERIOR:
            for(unsigned int i=0; i<flooding.size(); i++){
                if(planeNeighbours[flooding[i]]==-1){
                    // Get the triangles they belong to
                    for(unsigned int j=0; j<vertexTriangles[i].size(); j++){
                        // If it's not already in the list
                        if(!truthTriangles[vertexTriangles[i][j]]){
                            trianglesCut.push_back(vertexTriangles[i][j]);
                            truthTriangles[vertexTriangles[i][j]] = true;
                        }
                    }
                }
            }
            //for(int i=0; i<4; i++) std::cout << i << " (interior) : " << planeNeighbours[i] << std::endl;
        break;

            // fibula
        case Side::EXTERIOR:
            for(unsigned int i=0; i<flooding.size(); i++){
                bool isKeep = false;
                // Only keep it if it belongs to a kept segment
                for(int k=0; k<segmentsConserved.size(); k++){
                    if(segmentsConserved[k]==flooding[i]){
                        isKeep = true;
                        break;
                    }
                }
                if(isKeep){
                //if(planeNeighbours[flooding[i]]!= -1){
                    // Get the triangles they belong to
                    for(unsigned int j=0; j<vertexTriangles[i].size(); j++){
                        // If it's not already in the list
                        if(!truthTriangles[vertexTriangles[i][j]]){
                            trianglesCut.push_back(vertexTriangles[i][j]);
                            truthTriangles[vertexTriangles[i][j]] = true;
                        }
                    }
                }
            }
            //for(int i=0; i<2*planes.size(); i++) std::cout << i << " : " << planeNeighbours[i] << std::endl;
        break;
    }

    trianglesExtracted.clear();
    for(int i=0; i<triangles.size(); i++){
        if(!truthTriangles[i]) trianglesExtracted.push_back(i);
    }

    // ! Conserve this order
    if(cuttingSide == Side::EXTERIOR) getSegmentsToKeep();
    createSmoothedTriangles();
    if(cuttingSide == Side::EXTERIOR) sendToManible();

}

// WARNING : this assumes that the left and right planes are the first planes added!
// Could search for the exterior planes beforehand using the fact that the other sides = -1
void Mesh::getSegmentsToKeep(){
    segmentsConserved.clear();

    // Find the non-discarded side of the left plane
    int planeToKeep;
    if(flooding[0]!=-1) planeToKeep = 0;
    else planeToKeep = planes.size();   // keep the otherside if 0 is discared

    // while we haven't found the right plane
    while(planeToKeep!=1 && planeToKeep!=planes.size()+1){

        int nextPlane = planeNeighbours[planeToKeep];   // move on to the next plane

        // Keep the smaller of the two values to match the merge flood
        if(planeToKeep < nextPlane) segmentsConserved.push_back(planeToKeep);
        else segmentsConserved.push_back(nextPlane);

        // discard the other side
        int toDiscard;
        if( nextPlane < planes.size() ) toDiscard = nextPlane + planes.size();
        else toDiscard = nextPlane - planes.size();

        if(toDiscard==1 || toDiscard==planes.size()+1) break;

        // move on to the next plane
        nextPlane = planeNeighbours[toDiscard];

        // keep the other side
        if( nextPlane < planes.size() ) planeToKeep = nextPlane + planes.size();
        else planeToKeep = nextPlane - planes.size();
    }

    //for(int i=0; i<segmentsConserved.size(); i++) std::cout<< i << " : (conserved) " << segmentsConserved[i] << std::endl;
}

void Mesh::createSmoothedTriangles(){
    smoothedVerticies.clear();

    // Copy the verticies table
    for(unsigned int i=0; i<vertices.size(); i++){
        smoothedVerticies.push_back(vertices[i]);
    }

    switch (cuttingSide) {
        case Side::INTERIOR:
        for(unsigned long long i=0; i<planes.size(); i++){
            // for each triangle cut
            for(unsigned long long j=0; j<intersectionTriangles[static_cast<unsigned long long>(i)].size(); j++){
                 // find which verticies to keep
                for(unsigned int k=0; k<3; k++){
                    unsigned int vertexIndex = triangles[intersectionTriangles[i][j]].getVertex(k);
                    // here we only change it if it's outside of the cut (fine for mandible)
                    if(planeNeighbours[flooding[vertexIndex]] != -1){   // if we need to change it
                        Vec newVertex = planes[i]->getProjection(Vec(static_cast<double>(vertices[vertexIndex][0]), static_cast<double>(vertices[vertexIndex][1]), static_cast<double>(vertices[vertexIndex][2])) );
                        smoothedVerticies[vertexIndex] = Vec3Df(static_cast<float>(newVertex.x), static_cast<float>(newVertex.y), static_cast<float>(newVertex.z)); // get the projection
                    }
                    // else don't change the original
                }

            }
        }
        break;

        case Side::EXTERIOR:
            for(unsigned int i=0; i<planes.size(); i++){
                // for each triangle cut
                for(unsigned long long j=0; j<intersectionTriangles[static_cast<unsigned long long>(i)].size(); j++){
                    // Conserve the "real" flooding value
                    int actualFlooding = -1;    // will never stay at -1
                     // find which verticies are on the otherside of the cut
                    for(unsigned int k=0; k<3; k++){
                        unsigned int vertexIndex = triangles[intersectionTriangles[i][j]].getVertex(k);

                        bool isOutlier = true;
                        for(int l=0; l<segmentsConserved.size(); l++){
                            if(flooding[vertexIndex] == segmentsConserved[l]){
                                actualFlooding = flooding[vertexIndex];
                                isOutlier = false;
                            }
                        }

                        // if we need to change it
                        if(planeNeighbours[flooding[vertexIndex]]==-1 || isOutlier){
                            Vec newVertex = planes[i]->getProjection(Vec(static_cast<double>(vertices[vertexIndex][0]), static_cast<double>(vertices[vertexIndex][1]), static_cast<double>(vertices[vertexIndex][2])) );
                            smoothedVerticies[vertexIndex] = Vec3Df(static_cast<float>(newVertex.x), static_cast<float>(newVertex.y), static_cast<float>(newVertex.z)); // get the projection
                        }
                        // else don't change the original
                    }

                    // Set the whole triangle to the correct flooding value
                    for(unsigned int k=0; k<3; k++){
                        unsigned int vertexIndex = triangles[intersectionTriangles[i][j]].getVertex(k);
                        flooding[vertexIndex] = actualFlooding;
                    }
                }
            }
            break;

    }
}

void Mesh::updatePlaneIntersections(Plane *p){
    // Possible optimisation?

    updatePlaneIntersections();
}

void Mesh::floodNeighbour(unsigned int index, unsigned int id){
    // Flood it
    if(flooding[index] == -1){
        flooding[index] = id;

        for(unsigned int i=0; i<vertexNeighbours[index].size(); i++){
            floodNeighbour(vertexNeighbours[index][i], id);
        }
    }

    // stop if the vertex is already flooded with the same value
    else if(flooding[index] == id) return;
    // stop if we've found our own neg/pos side
    else if(flooding[index]==id+planes.size() || id==flooding[index]+planes.size()) return;

    // else it already belongs to a different plane
    else{
        // They're not already neighbours
        if(planeNeighbours[id]== -1){
            planeNeighbours[id] = flooding[index];     // equal to the old value
            planeNeighbours[flooding[index]] = id;
        }

        // They're already neighbours
        return;
    }
}

// Non permanent colour leak when they get too close, but they should never be allowed to be that close
void Mesh::mergeFlood(){
    for(int i=0; i<flooding.size(); i++){
        if(planeNeighbours[flooding[i]] != -1 && planeNeighbours[flooding[i]] < flooding[i]){
            // From the two neighbours, set them both to the lowest value
            flooding[i] = planeNeighbours[flooding[i]];
        }
    }
}

// Finds all the intersections for plane nb index
void Mesh::planeIntersection(unsigned int index){
    intersectionTriangles[index].clear();

    for(unsigned int i = 0 ; i < triangles.size(); i++){
        unsigned int t0 = triangles[i].getVertex(0);
        unsigned int t1 = triangles[i].getVertex(1);
        unsigned int t2 = triangles[i].getVertex(2);


        if(planes[index]->isIntersection(Vec(vertices[t0]), Vec(vertices[t1]), Vec(vertices[t2]) )){

            intersectionTriangles[index].push_back(i);

            // For each vertex, get the apporiate sign
            for(unsigned int j=0; j<3; j++){
                double sign = planes[index]->getSign(Vec(vertices[triangles[i].getVertex(j)]));
                if(sign >= 0 ){
                    flooding[triangles[i].getVertex(j)] = planes.size() + index;
                }
                else if(sign < 0){
                    flooding[triangles[i].getVertex(j)] = index;
                }
            }
        }
    }
}

void Mesh::sendToManible(){
    std::vector<int> planeNb;       // the plane nb associated
    std::vector<Vec> convertedVerticies;    // the vertex coordinates in relation to the plane nb
    std::vector<std::vector<int>>convertedTriangles; // the new indicies of the triangles (3 indicies)
    int tempVerticies[smoothedVerticies.size()];   // a temporary marker for already converted verticies

    for(int i=0; i<smoothedVerticies.size(); i++) tempVerticies[i] = -1;

    // For every triangle we want to send (we've already filtered out the rest when cutting the mesh)
    for(unsigned int i=0; i<trianglesCut.size(); i++){

        // Get the 3 verticies of the triangle
        unsigned int triVert;
        std::vector<int> newTriangle;

        for(int j=0; j<3; j++){
            triVert = triangles[trianglesCut[i]].getVertex(j);       // this must go into smoothedVerticies[triV....]

            // If converted already
            if(tempVerticies[triVert] != -1) newTriangle.push_back(tempVerticies[triVert]);
            // convert to the corresponding plane
            else{
                // Get the plane nb
                int pNb = flooding[triVert];

                if(pNb >= planes.size()) pNb -= planes.size();

                planeNb.push_back(pNb);

                // Convert the vertex
                Vec unConverted = Vec(smoothedVerticies[triVert][0], smoothedVerticies[triVert][1], smoothedVerticies[triVert][2]);
                //std::cout << "Unconverted : " << unConverted.x << " " << unConverted.y << " " << unConverted.z << std::endl;
                Vec convertedCoords = planes[pNb]->getLocalCoordinates(unConverted);
                //std::cout << "Converted : " << convertedCoords.x << " " << convertedCoords.y << " " << convertedCoords.z << std::endl;
                convertedVerticies.push_back(convertedCoords);
                int vertexIndex = convertedVerticies.size() - 1;
                newTriangle.push_back(vertexIndex);  // set it to the last index of convertedVerticies

                // Store the corresponding index in tempVerticies
                tempVerticies[triVert] = vertexIndex;
            }
        }

        // Add the triangle
        convertedTriangles.push_back(newTriangle);

    }

    /*for(int i=0; i<convertedTriangles.size(); i++){
        for(int j=0; j<3; j++){
            std::cout << i << " : (plane) " << planeNb[convertedTriangles[i][j]] << " , (vertice) : " << convertedVerticies[convertedTriangles[i][j]][0] << " " << convertedVerticies[convertedTriangles[i][j]][1] << " " << convertedVerticies[convertedTriangles[i][j]][2] << std::endl;
        }
    }*/

    // Need to send the three initial tables (+ a new table of normals to be dealt with later)
    Q_EMIT sendInfoToManible(planeNb, convertedVerticies, convertedTriangles);
}

void Mesh::recieveInfoFromFibula(std::vector<Vec> convertedVerticies, std::vector<std::vector<int>> convertedTriangles){
    if(cuttingSide != Side::INTERIOR) return;

    for(unsigned int i=0; i<convertedVerticies.size(); i++){
        Vec3Df v = Vec3Df(convertedVerticies[i].x, convertedVerticies[i].y, convertedVerticies[i].z);
        fibInMandVerticies.push_back(v);
    }

    for(unsigned int i=0; i<convertedTriangles.size(); i++){
        Triangle t = Triangle(convertedTriangles[i][0], convertedTriangles[i][1], convertedTriangles[i][2]);
        fibInMandTriangles.push_back(t);
    }
}

void Mesh::draw()
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glBegin (GL_TRIANGLES);

    if(!isCut){
        for(unsigned int i = 0 ; i < triangles.size(); i++){
            glTriangle(i);
        }
    }
    else{
        for(unsigned int i = 0 ; i < trianglesCut.size(); i++){
            glTriangleSmooth(trianglesCut[i]);
        }

        for(unsigned int i=0; i<fibInMandTriangles.size(); i++){
            glTriangleFibInMand(i);
        }
    }

    glEnd();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
}

void Mesh::drawCut(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    glBegin (GL_TRIANGLES);
    for(unsigned int i = 0 ; i < trianglesExtracted.size(); i++){
        glTriangleSmooth(trianglesExtracted[i]);
    }

    glEnd();

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
}

float Mesh::getBBRadius(){
    computeBB();
    return radius;
}
