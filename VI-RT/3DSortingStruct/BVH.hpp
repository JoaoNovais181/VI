#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <vector>
#include "AccelStruct.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "BB.hpp"
#include "primitive.hpp"
#include "triangle.hpp"
#include "mesh.hpp"

struct BVHNode {
    BB boundingBox;
    BVHNode *left, *right;
    Primitive *primitive;

    BVHNode() : left(nullptr), right(nullptr), primitive(nullptr) {}
};

struct BVHNodeGeo {
    BB boundingBox;
    BVHNodeGeo *left, *right;
    std::vector<Triangle*> triangles;
    int materialIndex;

    BVHNodeGeo() : left(nullptr), right(nullptr), materialIndex(-1) {}
};

class BVH : public AccelStruct {
private:
    int type;
    BVHNode *root;
    BVHNodeGeo *rootGeo;
    BVHNode *buildBVH(std::vector<Primitive*>& primitives, int depth);
    BVHNodeGeo *buildBVHGeoAux(std::vector<Triangle*>& triangles, int materialIndex, int depth);
    BVHNodeGeo *buildBVHGeo(std::vector<Primitive*>& primitives, int depth);
    bool traverseBVH(BVHNode *node, Ray &r, Intersection *isect);
    bool traverseBVHGeo(BVHNodeGeo* node, Ray& r, Intersection* isect);
    void deleteBVH(BVHNode* node);
    void deleteBVHGeo(BVHNodeGeo* node);

public:

    BVH(int _type=0): type(_type){}
    ~BVH(){deleteBVH(root); deleteBVHGeo(rootGeo);}
    void build(Scene *scene);
    bool trace (Ray r, Intersection *isect);
};

#endif