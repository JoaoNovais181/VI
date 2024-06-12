#include "BVH.hpp"
#include "scene.hpp"
#include <stdio.h>


void BVH::build(Scene* scene) {
    printf("Building Accel Struct..\n");
    this->scene = scene;
    auto prims = scene->getPrims();
    root = buildBVH(prims, 0);
    printf("Building Accel Struct Tris..\n");
    rootGeo = buildBVHGeo(prims, 0);
    printf("End\n");
}

bool BVH::traverseBVH(BVHNode* node, Ray& r, Intersection* isect) {
    if (!node || !node->boundingBox.intersect(r)) return false;

    Intersection leftIsect, rightIsect, curr_isect;

    bool hitLeft = traverseBVH(node->left, r, &leftIsect);
    if (hitLeft)
        *isect = leftIsect;

    bool hitRight = traverseBVH(node->right, r, &rightIsect);

    if (hitRight &&  (!hitLeft || leftIsect.depth > rightIsect.depth))
        *isect = rightIsect;

    bool hitPrimitive = false;
    if (node->primitive && node->primitive->g->intersect(r, &curr_isect)) {
        hitPrimitive = true;

        (*isect) = curr_isect;
        isect->f = this->scene->getMaterial(node->primitive->material_ndx);
    }

    return hitLeft || hitRight || hitPrimitive;
}

bool BVH::traverseBVHGeo(BVHNodeGeo* node, Ray& r, Intersection* isect) {
    if (!node || !node->boundingBox.intersect(r)) return false;

    Intersection leftIsect, rightIsect, curr_isect;

    bool hitLeft = traverseBVHGeo(node->left, r, &leftIsect);
    if (hitLeft)
        *isect = leftIsect;

    bool hitRight = traverseBVHGeo(node->right, r, &rightIsect);

    if (hitRight &&  (!hitLeft || leftIsect.depth > rightIsect.depth))
        *isect = rightIsect;

    bool hitPrimitive = false;
    for (auto prim_itr = node->triangles.begin(); prim_itr != node->triangles.end(); prim_itr++)
    {
        if ((*prim_itr)->intersect(r, &curr_isect))
        {
            if (!hitPrimitive)
            { // first intersection
                hitPrimitive = true;
                *isect = curr_isect;
            }
            else if (curr_isect.depth < isect->depth)
            {
                *isect = curr_isect;
            }
            isect->f = this->scene->getMaterial(node->materialIndex);
        }
    }
    // if (node->geometry && node->geometry->intersect(r, &curr_isect)) {
    //     hitPrimitive = true;

    //     (*isect) = curr_isect;
    //     isect->f = this->scene->getMaterial(node->materialIndex);
    // }

    return hitLeft || hitRight || hitPrimitive;
}

bool BVH::trace (Ray r, Intersection *isect) 
{
    return traverseBVHGeo(this->rootGeo, r, isect);
}


BVHNode *BVH::buildBVH(std::vector<Primitive *> &primitives, int depth)
{
    if (primitives.empty())
        return nullptr;

    BVHNode *node = new BVHNode();
    node->boundingBox = primitives[0]->g->bb;

    for (const auto &prim : primitives)
    {
        node->boundingBox.update(prim->g->bb);
    }

    if (primitives.size() == 1)
    {
        node->primitive = primitives[0];
        return node;
    }

    // split in the middle of the biggest axis
    Point min = node->boundingBox.min, max = node->boundingBox.max;

    float xsize = fabsf(max.X - min.X);
    float ysize = fabsf(max.Y - min.Y);
    float zsize = fabsf(max.Z - min.Z);

    int axis = 0;
    if (xsize >= ysize && xsize >= zsize)
        axis = 0; // X axis is the largest
    else if (ysize >= zsize)
        axis = 1; // Y axis is the largest
    else
        axis = 2; // Z axis is the largest

    auto comparator = [axis](Primitive *a, Primitive *b)
    {
        if (axis == 0)
            return a->g->bb.center().X < b->g->bb.center().X;
        else if (axis == 1)
            return a->g->bb.center().Y < b->g->bb.center().Y;
        return a->g->bb.center().Z < b->g->bb.center().Z;
    };
    std::sort(primitives.begin(), primitives.end(), comparator);

    size_t mid = primitives.size() / 2;
    std::vector<Primitive *> leftPrimitives(primitives.begin(), primitives.begin() + mid);
    std::vector<Primitive *> rightPrimitives(primitives.begin() + mid, primitives.end());

    node->left = buildBVH(leftPrimitives, depth + 1);
    node->right = buildBVH(rightPrimitives, depth + 1);

    return node;
}

BVHNodeGeo *BVH::buildBVHGeoAux(std::vector<Triangle*>& triangles, int materialIndex, int depth) {
    if (triangles.empty())
        return nullptr;

    BVHNodeGeo *node = new BVHNodeGeo();
    node->boundingBox = triangles[0]->bb;

    for (const auto &tri : triangles)
    {
        node->boundingBox.update(tri->bb);
    }

    if (triangles.size() <= 1)
    {
        node->triangles = triangles;
        // node->geometry = triangles[0];
        node->materialIndex = materialIndex;
        return node;
    }

    // split in the middle of the biggest axis
    Point min = node->boundingBox.min, max = node->boundingBox.max;

    float xsize = fabsf(max.X - min.X);
    float ysize = fabsf(max.Y - min.Y);
    float zsize = fabsf(max.Z - min.Z);

    int axis = 0;
    if (xsize >= ysize && xsize >= zsize)
        axis = 0; // X axis is the largest
    else if (ysize >= zsize)
        axis = 1; // Y axis is the largest
    else
        axis = 2; // Z axis is the largest

    auto comparator = [axis](Triangle *a, Triangle *b)
    {
        if (axis == 0)
            return a->middlePoint().X < b->middlePoint().X;
        else if (axis == 1)
            return a->middlePoint().Y < b->middlePoint().Y;
        return a->middlePoint().Z < b->middlePoint().Z;
    };
    std::sort(triangles.begin(), triangles.end(), comparator);

    size_t mid = triangles.size() / 2;
    std::vector<Triangle *> leftTriangles(triangles.begin(), triangles.begin() + mid);
    std::vector<Triangle *> rightTriangles(triangles.begin() + mid, triangles.end());

    node->left = buildBVHGeoAux(leftTriangles, materialIndex, depth + 1);
    node->right = buildBVHGeoAux(rightTriangles, materialIndex, depth + 1);

    return node;
}


BVHNodeGeo *BVH::buildBVHGeo(std::vector<Primitive*>& primitives, int depth) {
    if (primitives.empty())
        return nullptr;

    BVHNodeGeo *node = new BVHNodeGeo();
    node->boundingBox = primitives[0]->g->bb;

    for (const auto &prim : primitives)
    {
        node->boundingBox.update(prim->g->bb);
    }

    if (primitives.size() == 1)
    {
        if (dynamic_cast<Mesh*>(primitives[0]->g)) {
            Mesh *mesh = (Mesh*)primitives[0]->g;
            std::vector<Triangle*> triangles;

            for (Face face : mesh->faces) {
                Point v1 = mesh->vertices[face.vert_ndx[0]];
                Point v2 = mesh->vertices[face.vert_ndx[1]];
                Point v3 = mesh->vertices[face.vert_ndx[2]];
                Triangle *tri = new Triangle(v1, v2, v3, face.geoNormal);
                triangles.push_back(tri);
            }

            node = this->buildBVHGeoAux(triangles, primitives[0]->material_ndx ,depth+1);

        }
        return node;
    }

    // split in the middle of the biggest axis
    Point min = node->boundingBox.min, max = node->boundingBox.max;

    float xsize = fabsf(max.X - min.X);
    float ysize = fabsf(max.Y - min.Y);
    float zsize = fabsf(max.Z - min.Z);

    int axis = 0;
    if (xsize >= ysize && xsize >= zsize)
        axis = 0; // X axis is the largest
    else if (ysize >= zsize)
        axis = 1; // Y axis is the largest
    else
        axis = 2; // Z axis is the largest

    auto comparator = [axis](Primitive *a, Primitive *b)
    {
        if (axis == 0)
            return a->g->bb.center().X < b->g->bb.center().X;
        else if (axis == 1)
            return a->g->bb.center().Y < b->g->bb.center().Y;
        return a->g->bb.center().Z < b->g->bb.center().Z;
    };
    std::sort(primitives.begin(), primitives.end(), comparator);



    size_t mid = primitives.size() / 2;
    std::vector<Primitive *> leftPrimitives(primitives.begin(), primitives.begin() + mid);
    std::vector<Primitive *> rightPrimitives(primitives.begin() + mid, primitives.end());

    node->left = buildBVHGeo(leftPrimitives, depth + 1);
    node->right = buildBVHGeo(rightPrimitives, depth + 1);

    return node;
}


void BVH::deleteBVH(BVHNode* node) {
    if (!node) return;
    deleteBVH(node->left);
    deleteBVH(node->right);
    delete node;
}


void deleteBVHGeo(BVHNodeGeo* node);
