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

Vec3Df Mesh::computeTriangleNormal( int id ){

    const Triangle & t = triangles[id];
    Vec3Df normal = Vec3Df::crossProduct(vertices[t.getVertex (1)] - vertices[t.getVertex (0)], vertices[t.getVertex (2)]- vertices[t.getVertex (0)]);
    normal.normalize();
    return normal;

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

    // intersectionTriangles is sorted on creation
    /*for(int k=0; k<interIndex.size(); k++){ // check all the planes
        if(interIndex[k] < intersectionTriangles[k].size() && static_cast<unsigned int>(i) == intersectionTriangles[k][interIndex[k]]){
            //std::cout<< interIndex[k];
            interIndex[k]++;
            glColor3f(0, 0.5, 0.5);
        }
    }*/

    for( int j = 0 ; j < 3 ; j++ ){
        if(flooding[t.getVertex(j)] == 0) glColor3f(0, 0, 1);
        if(flooding[t.getVertex(j)] == 1) glColor3f(0, 0.5, 0.5);
        if(flooding[t.getVertex(j)] == planes.size()) glColor3f(1, 0, 0);
        if(flooding[t.getVertex(j)] == planes.size()+1) glColor3f(1, 1, 0);
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(vertices[t.getVertex(j)]);
    }

    glColor3f(1.0, 1.0, 1.0);
}

void Mesh::addPlane(Plane *p){
    //int index = planes.size();
    planes.push_back(p);
    planeNeighbours.push_back(-1);  // This is done once for the neg and once for the pos
    planeNeighbours.push_back(-1);
    std::vector<unsigned int> init;
    intersectionTriangles.push_back(init);
    //planeIntersection(index);     // get the intersections
    updatePlaneIntersections(p);
    interIndex.push_back(0);    // at zero by default
}

void Mesh::updatePlaneIntersections(){
    flooding.clear();
    for(int i=0; i<vertices.size(); i++) flooding.push_back(-1);

    for(int i=0; i<planes.size(); i++) planeIntersection(i);

    // Brute method
    for(int i=0; i<flooding.size(); i++){
        if(flooding[i] != -1){
            for(int j=0; j<vertexNeighbours[i].size(); j++)
            floodNeighbour(vertexNeighbours[i][j], flooding[i]);
        }
    }

    //std::cout << planeNeighbours.size() << std::endl;

    /*int i=0;
    while (i<planeNeighbours.size()){
        std::cout << planeNeighbours[i] << "," << planeNeighbours[i+1] << std::endl;
        i+=2;
    }*/

    //mergeFlood();
}

void Mesh::updatePlaneIntersections(Plane *p){
    /*flooding.clear();
    for(int i=0; i<vertices.size(); i++) flooding.push_back(-1);*/

    /*for(int i=0; i<planes.size(); i++){
        if(p == planes[i]){
            for(int j=0; j<vertices.size(); j++){
                if(flooding[j] == i || flooding[j] == i + planes.size()) flooding[j] = -1;      // Reinitialise the plane
            }
            planeIntersection(i);
        }
    }

    // Brute method
    for(int i=0; i<flooding.size(); i++){
        if(flooding[i] != -1){
            for(int j=0; j<vertexNeighbours[i].size(); j++)
            floodNeighbour(vertexNeighbours[i][j], flooding[i]);
        }
    }*/

    updatePlaneIntersections();

    //mergeFlood();
}

void Mesh::floodNeighbour(int index, int id){
    // Uninitialised
    if(flooding[index] == -1){
        flooding[index] = id;

        for(int i=0; i<vertexNeighbours[index].size(); i++){
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
        if(planeNeighbours[id]==-1){
            /*std::cout << "Before " << planeNeighbours[id] << std::endl;
            std::cout << "Flooding " << flooding[index] << std::endl;
            std::cout << "Id " <<  id << std::endl;*/

            planeNeighbours[id] = flooding[index];     // equal to the old value
            planeNeighbours[flooding[index]] = id;

            //std::cout << " " << std::endl;
            return;
        }

        // They're already neighbours
        return;

        /*int pNindex1, pNindex2;

        // Extract the first planeNeighbour index from the id (the new plane)
        if(id > planes.size()-1){
            pNindex1 = 2 * (id - planes.size()) + 1;  // it's positive
        }
        else pNindex1 = 2 * id;

        // They're not already neighbours
        if(planeNeighbours[pNindex1]==-1){

            std::cout << "Old " << pNindex1 << " = " << planeNeighbours[pNindex1] << std::endl;

            if(flooding[index] > planes.size()-1){
                pNindex2 = 2 * (flooding[index] - planes.size()) + 1;  // it's positive
            }
            else pNindex2 = 2 * flooding[index];

            std::cout << "First " << pNindex1 << " from " << id << " = " << flooding[index] << std::endl;
            std::cout << "Second " << pNindex2 << " from " << flooding[index] << " = " << id << std::endl;
            std::cout << " "<< std::endl;

            planeNeighbours[pNindex1] = flooding[index];
            planeNeighbours[pNindex2] = id;
            return;
        }
        // They're already neighbours
        else if(planeNeighbours[pNindex1]!=-1) return;*/
    }
}

void Mesh::mergeFlood(){
    for(int i=0; i<flooding.size(); i++){
        if(planeNeighbours[flooding[i]] < flooding[i]) flooding[i] = planeNeighbours[flooding[i]];
    }
}

// Finds all the intersections for plane nb index
void Mesh::planeIntersection(int index){
    intersectionTriangles[index].clear();

    for(unsigned int i = 0 ; i < triangles.size(); i++){
        unsigned int t0 = triangles[i].getVertex(0);
        unsigned int t1 = triangles[i].getVertex(1);
        unsigned int t2 = triangles[i].getVertex(2);
        if(planes[index]->isIntersection(Vec(vertices[t0]), Vec(vertices[t1]), Vec(vertices[t2]) )){
            intersectionTriangles[index].push_back(i);

            for(int j=0; j<3; j++){
                int sign = planes[index]->getSign(Vec(vertices[triangles[i].getVertex(j)]));
                if(sign == 1) flooding[triangles[i].getVertex(j)] = planes.size() + index;
                else if(sign == -1) flooding[triangles[i].getVertex(j)] = index;
            }
        }
    }

    //std::cout << intersectionTriangles[index].size() << std::endl;
}

void Mesh::draw()
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glBegin (GL_TRIANGLES);
    for(unsigned int i = 0 ; i < interIndex.size(); i++) interIndex[i] = 0;
    for(unsigned int i = 0 ; i < triangles.size(); i++){
        glTriangle(i);
    }
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
}

float Mesh::getBBRadius(){
    computeBB();
    return radius;
}
