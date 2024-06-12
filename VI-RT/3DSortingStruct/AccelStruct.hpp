//
//  AccelStruct.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 31/01/2023.
//

#ifndef AccelStruct_hpp
#define AccelStruct_hpp

#include <algorithm>
#include <vector>
#include "ray.hpp"
#include "intersection.hpp"
#include "BB.hpp"
#include "primitive.hpp"
#include "triangle.hpp"
#include "mesh.hpp"


class Scene;

class AccelStruct {

public:
    AccelStruct () {}
    ~AccelStruct () {}
    virtual void build (Scene *s) = 0;
    virtual bool trace (Ray r, Intersection *isect) = 0;

protected:
    Scene *scene;
};

#endif /* AccelStruct_hpp */
