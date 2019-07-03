#ifndef MESH_H
#define MESH_H

#include "Vec3D.h"
#include "Triangle.h"
#include <queue>

enum Axis {X, Y, Z};

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

    void draw();

    void recomputeNormals();
    void update();
    void clear();

    float getBBRadius();

    typedef std::priority_queue< std::pair< float , int > , std::deque< std::pair< float , int > > , std::greater< std::pair< float , int > > > FacesQueue;

    void invertNormal(){normalDirection *= -1;}
protected:
    void init();
    void computeBB();

    void computeTriangleNormals();
    Vec3Df computeTriangleNormal(int t);
    void computeVerticesNormals();
    void glTriangle(unsigned int i);

    std::vector <Vec3Df> vertices;
    std::vector <Triangle> triangles;

    std::vector<Vec3Df> normals;
    std::vector<Vec3Df> verticesNormals;

    Vec3Df BBMin;
    Vec3Df BBMax;
    Vec3Df BBCentre;
    float radius;

    int normalDirection;
};

#endif // MESH_H

