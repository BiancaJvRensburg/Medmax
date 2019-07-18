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
        /* Visualisation testing */
               if(flooding[t.getVertex(j)] == 0) glColor3f(0, 0, 1);
               if(flooding[t.getVertex(j)] == 1) glColor3f(0, 0.5, 0.5);
               if(flooding[t.getVertex(j)] == planes.size()) glColor3f(1, 0, 0);
               if(flooding[t.getVertex(j)] == planes.size()+1) glColor3f(1, 1, 0);
               /* End of visualisation testing */
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(vertices[t.getVertex(j)]);
    }

    glColor3f(1.0, 1.0, 1.0);
}

void Mesh::glTriangleSmooth(unsigned int i){
    const Triangle & t = triangles[i];

    for(unsigned int j = 0 ; j < 3 ; j++ ){
        /* Visualisation testing */
               if(flooding[t.getVertex(j)] == 0) glColor3f(0, 0, 1);
               if(flooding[t.getVertex(j)] == 1) glColor3f(0, 0.5, 0.5);
               if(flooding[t.getVertex(j)] == planes.size()) glColor3f(1, 0, 0);
               if(flooding[t.getVertex(j)] == planes.size()+1) glColor3f(1, 1, 0);
               /* End of visualisation testing */
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(smoothedVerticies[t.getVertex(j)]);
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
    interIndex.push_back(0);    // at zero by default
}

void Mesh::updatePlaneIntersections(){
    //if(isCut){
        flooding.clear();
        for(int i=0; i<vertices.size(); i++) flooding.push_back(-1);
        for(int i=0; i<planeNeighbours.size(); i++) planeNeighbours[i] = -1;

        for(unsigned int i=0; i<planes.size(); i++) planeIntersection(i);

        // std::cout << " \n " << std::endl;

        //neighCount=0;
        //int count=0;
        for(unsigned int i=0; i<flooding.size(); i++){
            if(flooding[i] != -1){
                //count++;
                for(unsigned int j=0; j<vertexNeighbours[i].size(); j++)
                floodNeighbour(vertexNeighbours[i][j], flooding[i]);
            }
        }


        /*for(unsigned int i=0; i<planeNeighbours.size(); i+=2){
            std::cout << i << " : " << planeNeighbours[i] << " , " << planeNeighbours[i+1] << std::endl;
        }*/

        mergeFlood();
        if(isCut)cutMesh();
    //}
}

void Mesh::cutMesh(){
    //isCut = true;
    trianglesCut.clear();

    bool truthTriangles[triangles.size()];  // keeps a record of the triangles who are already added
    for(unsigned int i=0; i<triangles.size(); i++) truthTriangles[i] = false;

    //int count = 0;
    switch (cuttingSide) {
        case Side::INTERIOR:
            for(unsigned int i=0; i<flooding.size(); i++){
                if(planeNeighbours[flooding[i]]==-1){
                    // Get the triangles they belong to
                    //count++;
                    for(unsigned int j=0; j<vertexTriangles[i].size(); j++){
                        // If it's not already in the list
                        if(!truthTriangles[vertexTriangles[i][j]]){
                            trianglesCut.push_back(vertexTriangles[i][j]);
                            truthTriangles[vertexTriangles[i][j]] = true;
                        }
                    }
                }
        }
        break;

        case Side::EXTERIOR:
            for(unsigned int i=0; i<flooding.size(); i++){
                if(planeNeighbours[flooding[i]]!= -1){
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
        break;
    }

    //std::cout << trianglesCut.size() << "   " << count << std::endl;

    createSmoothedTriangles();

}

void Mesh::createSmoothedTriangles(){
    smoothedVerticies.clear();

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
                 // find which verticies to keep
                for(unsigned int k=0; k<3; k++){
                    unsigned int vertexIndex = triangles[intersectionTriangles[i][j]].getVertex(k);
                    if(planeNeighbours[flooding[vertexIndex]] ==  -1){   // if we need to change it
                        Vec newVertex = planes[i]->getProjection(Vec(static_cast<double>(vertices[vertexIndex][0]), static_cast<double>(vertices[vertexIndex][1]), static_cast<double>(vertices[vertexIndex][2])) );
                        smoothedVerticies[vertexIndex] = Vec3Df(static_cast<float>(newVertex.x), static_cast<float>(newVertex.y), static_cast<float>(newVertex.z)); // get the projection
                    }
                    // else don't change the original
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
            //std::cout << i << std::endl;
            intersectionTriangles[index].push_back(i);

            for(unsigned int j=0; j<3; j++){
                int sign = planes[index]->getSign(Vec(vertices[triangles[i].getVertex(j)]));
                if(sign == 1) flooding[triangles[i].getVertex(j)] = planes.size() + index;
                else if(sign == -1) flooding[triangles[i].getVertex(j)] = index;
            }
        }
    }
}

void Mesh::draw()
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glBegin (GL_TRIANGLES);
    for(unsigned int i = 0 ; i < interIndex.size(); i++) interIndex[i] = 0;

    if(!isCut){
        for(unsigned int i = 0 ; i < triangles.size(); i++){
            glTriangle(i);
        }
    }
    else{
        for(unsigned int i = 0 ; i < trianglesCut.size(); i++){
            glTriangleSmooth(trianglesCut[i]);
        }
    }

    glEnd();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
}

float Mesh::getBBRadius(){
    computeBB();
    return radius;
}
