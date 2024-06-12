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

class Triangle: public Geometry {
public:
    Point v1, v2, v3;
    Vector normal;           // geometric normal
    Vector edge1, edge2;
    BB bb;      // face bounding box
                // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
    bool intersect (Ray r, Intersection *isect);
    bool isInside(Point p);

    Point middlePoint() {
        return (v1+v2+v3) / 3.0f;
    }
    
    Triangle(Point _v1, Point _v2, Point _v3, Vector _normal): v1(_v1), v2(_v2), v3(_v3), normal(_normal) {
        edge1 = v1.vec2point(v2);
        edge2 = v1.vec2point(v3);
        bb.min.set(v1.X, v1.Y, v1.Z);
        bb.max.set(v1.X, v1.Y, v1.Z);
        bb.update(v2);
        bb.update(v3);
    }

    Triangle(const Triangle& other): v1(other.v1), v2(other.v2), v3(other.v3), normal(other.normal), edge1(other.edge1), edge2(other.edge2), bb(other.bb) {}
    
    // Heron's formula
    // https://www.mathopenref.com/heronsformula.html
    float area () {
        
        Vector side1 = v1.vec2point(v2);
        Vector side2 = v1.vec2point(v3);
        Vector side3 = v2.vec2point(v3);
        
        float a = side1.norm(),
                b = side2.norm(),
                c = side3.norm(),
                p = (a+b+c)/2;
        //const float A = ...
        const float A = sqrtf(p * (p-a) * (p-b) * (p-c));

        return A;
    }
    float points_area (Point v1, Point v2, Point v3) {
        
        Vector side1 = v1.vec2point(v2);
        Vector side2 = v1.vec2point(v3);
        Vector side3 = v2.vec2point(v3);
        
        float a = side1.norm(),
                b = side2.norm(),
                c = side3.norm(),
                p = (a+b+c)/2;
        //const float A = ...
        const float A = sqrtf(p * (p-a) * (p-b) * (p-c));

        return A;
    }
};

#endif /* triangle_hpp */
