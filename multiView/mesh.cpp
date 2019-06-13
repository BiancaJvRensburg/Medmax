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

/*void Mesh::Zero(){
    computeBB();

    for(int i=0; i<vertices.size(); i++){
        vertices[i] -= BBCentre;
    }
} */

void Mesh::update(){
    computeBB();
    recomputeNormals();
    std::cout << "Mesh : " << vertices.size() << " vertices, " << triangles.size() << " triangles " << std::endl;
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

void Mesh::glTriangle(unsigned int i){

    const Triangle & t = triangles[i];
    for( int j = 0 ; j < 3 ; j++ ){
        //std::cout << "Normal : " << verticesNormals.size() << std::endl;
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(vertices[t.getVertex(j)]);
    }

}

Axis Mesh::computeAxis(){
    float x = abs(BBMax[0] - BBMin[0]);
    float y = abs(BBMax[1] - BBMin[1]);
    float z = abs(BBMax[2] - BBMin[2]);

    if(x>y && x>z) return Axis::X;
    else if(y>z) return Axis::Y;
    else return Axis::Z;
}

void Mesh::draw()
{
    recomputeNormals();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glBegin (GL_TRIANGLES);
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
