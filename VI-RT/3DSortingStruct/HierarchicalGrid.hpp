#ifndef HIERARCHICALGRID_H
#define HIERARCHICALGRID_H

#include <vector>
#include <algorithm>
#include "AccelStruct.hpp"
#include "primitive.hpp" // Assuming this contains your Primitive and Triangle definitions
#include "ray.hpp" // Assuming this contains your Ray definition
#include "intersection.hpp" // Assuming this contains your Intersection definition

class Scene;

struct GridCell;

struct GridCell {
    std::vector<Primitive*> primitives;
    int depth;
    GridCell* subgrid[3][3][3] = {nullptr}; // Subgrid cells
    float cellSizeX, cellSizeY, cellSizeZ; // Size of each cell
    BB boundingBox;

    GridCell(int _depth) 
    :primitives(), depth(_depth), boundingBox(), cellSizeX(0), cellSizeY(0), cellSizeZ(0)
    {}

    void calculateSizes();
};

class HierarchicalGrid : public AccelStruct {
public:
    HierarchicalGrid(int size):rootCell() {}
    ~HierarchicalGrid() {}

    void build(Scene *scene);
    bool trace(Ray ray, Intersection* isect);

private:
    GridCell* rootCell;
    int maxDepth = 3;
    void buildSubgrid(GridCell* cell, const std::vector<Primitive*>& primitives, int level);
    bool intersectSubgrid(GridCell* cell, Ray& ray, Intersection* isect);
};

#endif // HIERARCHICALGRID_H
