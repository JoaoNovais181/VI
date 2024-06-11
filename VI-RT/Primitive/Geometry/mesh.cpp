//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"

// see pbrt book (3rd ed.), sec 3.6.2, pag 157
//
// Suggestion: use:
// // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm

#include <stdio.h>

bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {
    if (!bb.intersect(r)) return false;

    Point p1 = this->vertices[f.vert_ndx[0]];
    Point p2 = this->vertices[f.vert_ndx[1]];
    Point p3 = this->vertices[f.vert_ndx[2]];
    Vector edge1 = p1.vec2point(p2);
    Vector edge2 = p1.vec2point(p3);

    Vector ray_cross_e2 = r.dir.cross(edge2);
    float det = edge1.dot(ray_cross_e2);

    if (det > -EPSILON && det < EPSILON)
        return false;

    
    float inv_det = 1.0 / det;
    Vector s = p1.vec2point(r.o);
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
        Vector normal = f.geoNormal;
        Vector wo = -1.f * r.dir;
        // make sure the normal points to the same side of the surface as wo
        normal.Faceforward(wo);
        isect->gn = normal;
        isect->sn = normal;
        isect->p = r.o  + r.dir * t;
        isect->wo = wo;
        isect->FaceID = f.FaceID;
        isect->isLight = false;
        isect->depth = t;
        return true;
    }

    return false;
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = true, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth=MAXFLOAT;
    // intersect the ray with the mesh BB
    if (!bb.intersect(r)) return false;
    
    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        intersect_this_face = TriangleIntersect(r, *face_it.base(), &curr_isect);
        if (!intersect_this_face) continue;
        
        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }

    isect->p = min_isect.p;
    isect->wo = min_isect.wo;
    isect->gn = min_isect.gn;
    isect->sn = min_isect.sn;
    isect->FaceID = min_isect.FaceID;
    isect->depth = min_isect.depth;
    isect->isLight = false;

    return intersect;
}
