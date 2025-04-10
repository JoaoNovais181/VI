//
//  Intersection.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Intersection_hpp
#define Intersection_hpp

#include "vector.hpp"
#include "BRDF.hpp"

typedef struct Intersection {
public:
    Point p;    // intersection point
    Vector gn;  // geometric normal
    Vector sn;  // shading normal (the same as gn for the time being)
    Vector wo;  // out direction
    float depth; // ray tree depth
    BRDF *f;     // pointer to the material
    int pix_x, pix_y;
    int FaceID;  // ID of the intersected face 
    bool isLight;  // for intersections with light sources
    RGB Le;         // for intersections with light sources
    
    
    Intersection() {}
    // from pbrt book, section 2.10, pag 116
    Intersection(const Point &p, const Vector &n, const Vector &wo, const float &depth)
    : p(p), gn(n), sn(n), wo(wo), depth(depth), f(NULL) { }
} Intersection;

#endif /* Intersection_hpp */
