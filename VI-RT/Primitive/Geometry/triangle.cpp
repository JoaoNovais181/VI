//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "triangle.hpp"
#include "BB.hpp"
#include <stdio.h>

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm
bool Triangle::intersect(Ray r, Intersection *isect)
{

    if (!bb.intersect(r))
    {
        return false;
    }
    Vector ray_cross_e2 = r.dir.cross(edge2);
    float det = edge1.dot(ray_cross_e2);

    if (det > -EPSILON && det < EPSILON)
        return false;

    float inv_det = 1.0 / det;
    Vector s = v1.vec2point(r.o);
    float u = inv_det * s.dot(ray_cross_e2); // first barycentric coordinate

    if (u < 0 || u > 1)
        return false;

    Vector s_cross_e1 = s.cross(edge1);
    float v = inv_det * r.dir.dot(s_cross_e1); // second barycentric coordinate

    if (v < 0 || u + v > 1)
        return false;

    // compute t to find the intersection point
    float t = inv_det * edge2.dot(s_cross_e1);

    if (t > EPSILON) // intersection
    {

        // Fill Intersection data
        Vector wo = -1.f * r.dir;
        // make sure the normal points to the same side of the surface as wo
        normal.Faceforward(wo);
        isect->gn = normal;
        isect->sn = normal;
        isect->p = r.o + r.dir * t;
        isect->wo = wo;
        isect->FaceID = -1;
        isect->isLight = false;
        isect->depth = t;
        return true;
    }

    return false;
}

bool Triangle::isInside(Point p)
{
    /* Calculate area of this triangle ABC */
    float A = area();

    /* Calculate area of triangle p v1 v2 */
    float A1 = points_area(p, v1, v2);
    /* Calculate area of triangle p v2 v3 */
    float A2 = points_area(p, v2, v3);
    /* Calculate area of triangle p v3 v1 */
    float A3 = points_area(p, v3, v1);

    /* Check if sum of A1, A2 and A3 is same as A */
    return (A == A1 + A2 + A3);
}
