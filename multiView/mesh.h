#ifndef MESH_H
#define MESH_H

#include "Vec3D.h"
#include "Triangle.h"
#include "plane.h"
#include <queue>

class Mesh
{
public:

    Mesh():normalDirection(1.){}
    Mesh(std::vector<Vec3Df> &vertices, std::vector<Triangle> &triangles): vertices(vertices), triangles(triangles), normalDirection(1.){
        update();
    }
    ~Mesh(){}

    std::vector<Vec3Df> &getVertices(){return vertices;}
    const std::vector<Vec3Df> &getVertices()const {return vertices;}

    std::vector<Triangle> &getTriangles(){return triangles;}
    const std::vector<Triangle> &getTriangles()const {return triangles;}

    std::vector< std::vector<int>> &getVertexNeighbours(){return vertexNeighbours;}
    const std::vector< std::vector<int>> &getVertexNeighbours()const {return vertexNeighbours;}

    void draw();

    void recomputeNormals();
    void update();
    void clear();

    float getBBRadius();

    void updatePlaneIntersections();    // need one for a single plane
    void updatePlaneIntersections(Plane *p);
    void addPlane(Plane *p);

    typedef std::priority_queue< std::pair< float , int > , std::deque< std::pair< float , int > > , std::greater< std::pair< float , int > > > FacesQueue;

    void invertNormal(){normalDirection *= -1;}
protected:
    void init();
    void computeBB();

    void computeTriangleNormals();
    Vec3Df computeTriangleNormal(int t);
    void computeVerticesNormals();
    void glTriangle(unsigned int i);

    void planeIntersection(int index);

    void floodNeighbour(int index, int id);     // flood the neighbours of the vertex index with the value id

    std::vector <Vec3Df> vertices;
    std::vector <Triangle> triangles;

    std::vector <Plane*> planes;
    std::vector <std::vector <unsigned int>> intersectionTriangles;    // Contains the index of the triangle instead of the actual triangle
    std::vector <unsigned int> interIndex;    // the current index of intersectionTriangles

    std::vector <int> flooding;
    std::vector< std::vector<int>> vertexNeighbours;
    std::vector<int> planeNeighbours;

    std::vector<Vec3Df> normals;
    std::vector<Vec3Df> verticesNormals;

    Vec3Df BBMin;
    Vec3Df BBMax;
    Vec3Df BBCentre;
    float radius;

    int normalDirection;
};

#endif // MESH_H

