//
//  Scene.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <iostream>
#include <string>
#include <vector>
#include "primitive.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "BRDF.hpp"

class HierarchicalGrid;

class AccelStruct;

class rehash {
public:
    int objNdx, ourNdx;
    friend bool operator< (rehash const& a, rehash const& b) {
        return a.objNdx < b.objNdx;
    }
};

class Scene {
    std::vector <Primitive *> prims;
    std::vector <BRDF *> BRDFs;
    AccelStruct *accelStruct;
public:
    std::vector <Light *> lights;
    int numPrimitives, numLights, numBRDFs;

    Scene ();
    Scene (bool generateAccelStruct);
    bool Load (const std::string &fname);
    bool SetLights (void) { return true; };
    bool trace (Ray r, Intersection *isect);
    bool visibility (Ray s, const float maxL);
    void printSummary(void) {
        std::cout << "#primitives = " << numPrimitives << " ; ";
        std::cout << "#lights = " << numLights << " ; ";
        std::cout << "#materials = " << numBRDFs << " ;" << std::endl;
    }
    std::vector <Primitive *> getPrims() {return this->prims;}
    BRDF *getMaterial(int indx) { return BRDFs[indx]; }
    void printScene();
};

#endif /* Scene_hpp */
