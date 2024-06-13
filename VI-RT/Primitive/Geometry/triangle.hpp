//
//  mesh.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#ifndef triangle_hpp
#define triangle_hpp

#include "geometry.hpp"
#include "vector.hpp"
#include <math.h>

class Triangle : public Geometry
{
public:
    Point v1, v2, v3;
    Vector normal; // geometric normal
    Vector edge1, edge2;
    BB bb; // face bounding box
           // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
    bool intersect(Ray r, Intersection *isect);
    bool isInside(Point p);

    Point middlePoint()
    {
        return (v1 + v2 + v3) / 3.0f;
    }

    Triangle(Point _v1, Point _v2, Point _v3, Vector _normal) : v1(_v1), v2(_v2), v3(_v3), normal(_normal)
    {
        edge1 = v1.vec2point(v2);
        edge2 = v1.vec2point(v3);
        bb.min.set(v1.X, v1.Y, v1.Z);
        bb.max.set(v1.X, v1.Y, v1.Z);
        bb.update(v2);
        bb.update(v3);
    }

    Triangle(const Triangle &other) : v1(other.v1), v2(other.v2), v3(other.v3), normal(other.normal), edge1(other.edge1), edge2(other.edge2), bb(other.bb) {}

    // Heron's formula
    // https://www.mathopenref.com/heronsformula.html
    float area()
    {

        Vector side1 = v1.vec2point(v2);
        Vector side2 = v1.vec2point(v3);
        Vector side3 = v2.vec2point(v3);

        float a = side1.norm(),
              b = side2.norm(),
              c = side3.norm(),
              p = (a + b + c) / 2;
        // const float A = ...
        const float A = sqrtf(p * (p - a) * (p - b) * (p - c));

        return A;
    }
    float points_area(Point v1, Point v2, Point v3)
    {

        Vector side1 = v1.vec2point(v2);
        Vector side2 = v1.vec2point(v3);
        Vector side3 = v2.vec2point(v3);

        float a = side1.norm(),
              b = side2.norm(),
              c = side3.norm(),
              p = (a + b + c) / 2;
        // const float A = ...
        const float A = sqrtf(p * (p - a) * (p - b) * (p - c));

        return A;
    }
    bool intersects(const BB &box) const
    {
        if (std::max({v1.X, v2.X, v3.X}) < box.min.X ||
            std::min({v1.X, v2.X, v3.X}) > box.max.X ||
            std::max({v1.Y, v2.Y, v3.Y}) < box.min.Y ||
            std::min({v1.Y, v2.Y, v3.Y}) > box.max.Y ||
            std::max({v1.Z, v2.Z, v3.Z}) < box.min.Z ||
            std::min({v1.Z, v2.Z, v3.Z}) > box.max.Z)
        {
            return false;
        }

        // Check if the box is completely outside the triangle's plane
        Vector boxVertices[8] = {
            {box.min.X, box.min.Y, box.min.Z}, {box.max.X, box.min.Y, box.min.Z}, {box.min.X, box.max.Y, box.min.Z}, {box.max.X, box.max.Y, box.min.Z}, {box.min.X, box.min.Y, box.max.Z}, {box.max.X, box.min.Y, box.max.Z}, {box.min.X, box.max.Y, box.max.Z}, {box.max.X, box.max.Y, box.max.Z}};

        Vector triangleEdges[3] = {
            v2 - v1,
            v3 - v2,
            v1 - v3};

        // Check box normals (x, y, z axes)
        Vector boxNormals[3] = {
            {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

        // Check triangle normal
        Vector triangleNormal = triangleEdges[0].cross(triangleEdges[1]);

        // Check edge cross products
        Vector edgeCrossNormals[9];
        int index = 0;
        for (const auto &edge : triangleEdges)
        {
            for (const auto &normal : boxNormals)
            {
                edgeCrossNormals[index++] = edge.cross(normal);
            }
        }

        auto testAxis = [&](const Vector &axis)
        {
            float triMin = std::min({v1.dot(axis), v2.dot(axis), v3.dot(axis)});
            float triMax = std::max({v1.dot(axis), v2.dot(axis), v3.dot(axis)});
            float boxMin = std::min({boxVertices[0].dot(axis), boxVertices[1].dot(axis), boxVertices[2].dot(axis), boxVertices[3].dot(axis),
                                     boxVertices[4].dot(axis), boxVertices[5].dot(axis), boxVertices[6].dot(axis), boxVertices[7].dot(axis)});
            float boxMax = std::max({boxVertices[0].dot(axis), boxVertices[1].dot(axis), boxVertices[2].dot(axis), boxVertices[3].dot(axis),
                                     boxVertices[4].dot(axis), boxVertices[5].dot(axis), boxVertices[6].dot(axis), boxVertices[7].dot(axis)});

            if (triMax < boxMin || triMin > boxMax)
            {
                return false;
            }
            return true;
        };

        for (const auto &axis : boxNormals)
        {
            if (!testAxis(axis))
            {
                return false;
            }
        }

        if (!testAxis(triangleNormal))
        {
            return false;
        }

        for (const auto &axis : edgeCrossNormals)
        {
            if (!testAxis(axis))
            {
                return false;
            }
        }

        return true;
    }
};

#endif /* triangle_hpp */
