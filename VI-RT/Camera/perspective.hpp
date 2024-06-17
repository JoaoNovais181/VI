//
//  perspective.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#ifndef perspective_hpp
#define perspective_hpp

#include "camera.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include "stdio.h"

class Perspective: public Camera {
    Point Eye, At;
    Vector Up;
    float fovW, fovH;
    int W, H;
    float c2w[3][3];  // camera 2 world transform
    void recalculateC2W() {
        // compute camera 2 world transform
        Vector F = Point(Eye).vec2point(At);
        F.normalize();
        Vector R = F.cross(Up);
        R.normalize();
        Vector U = R.cross(F);
        U.normalize();
        // Vector U = R.cross(F);
        // U.normalize();
        this->Up.normalize();
        this->c2w[0][0] = R.X;
        this->c2w[0][1] = R.Y;
        this->c2w[0][2] = R.Z;
        this->c2w[1][0] = Up.X;
        this->c2w[1][1] = Up.Y;
        this->c2w[1][2] = Up.Z;
        this->c2w[2][0] = F.X;
        this->c2w[2][1] = F.Y;
        this->c2w[2][2] = F.Z;
    }
public:
    Perspective (const Point Eye, const Point At, const Vector Up, const int W, const int H, const float fovW, const float fovH): Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH)  {
        this->Up.normalize();
        recalculateC2W();
    }
    bool GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter=NULL);
    void getResolution (int *_W, int *_H) {*_W=W; *_H=H;}
    void addEye(Vector vec) {Eye= Eye + vec;}
    Point getEye() {return Eye;}
    Point getAt() {return At;}
    Vector getUp() {return Up;}
    void moveFPS(Vector movement) {Eye = Eye+movement; At = At+movement; this->recalculateC2W();}
    void setAt(Point _At) {At = _At;this->recalculateC2W();}
};

#endif /* perspective_hpp */
