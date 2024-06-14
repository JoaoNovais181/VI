//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include <iostream>
#include "scene.hpp"
#include "perspective.hpp"
#include "StandardRenderer.hpp"
#include "WindowRenderer.hpp"
#include "ImagePPM.hpp"
#include "AmbientShader.hpp"
#include "WhittedShader.hpp"
#include "DistributedShader.hpp"
#include "PathTracerShader.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"

#include <time.h>
#include "mesh.hpp"

int main(int argc, const char * argv[]) {
    Scene scene(true);
    Perspective *cam; // Camera
    ImagePPM *img;    // Image
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;
    
    // success = scene.Load("VI-RT/Scene/tinyobjloader/models/cornell_box_VI.obj");
    success = scene.Load("/home/tbag/Downloads/multiCornellBox.obj");
    
    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    
    // add an ambient light to the scene
    // AmbientLight ambient(RGB(0.15,0.15,0.15));
    // scene.lights.push_back(&ambient);
    // scene.numLights++;
    // PointLight *pl1 = new PointLight(RGB(1,1,1), Point(0,18,0));
    // scene.lights.push_back(pl1);
    // scene.numLights++;
    // AreaLight *al1 = new AreaLight(RGB(1, 1, 1), 
    //                                 Point(343.0, 548.0, 227.0),
    //                                 Point(343.0, 548.0, 332.0),
    //                                 Point(213.0, 548.0, 332.0),
    //                                 Vector(0, -1, 0));
    // scene.lights.push_back(al1);
    // scene.numLights++;
    // AreaLight *al2 = new AreaLight(RGB(1, 1, 1), 
    //                                 Point(343.0, 548.0, 227.0),
    //                                 Point(213.0, 548.0, 332.0),
    //                                 Point(213.0, 548.0, 227.0),
    //                                 Vector(0, -1, 0));
    // scene.lights.push_back(al2);
    // scene.numLights++;

    AreaLight *al1 = new AreaLight(RGB(0.7, 0.7, .7), 
                                    Point(18, 27.9, 10),
                                    Point(18, 27.9, 18),
                                    Point(10, 27.9, 18),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al1);
    scene.numLights++;
    AreaLight *al2 = new AreaLight(RGB(0.7, 0.7, .7), 
                                    Point(18, 27.9, 10),
                                    Point(10, 27.9, 18),
                                    Point(10, 27.9, 10),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al2);
    scene.numLights++;

    AreaLight *al3 = new AreaLight(RGB(0.7, 0.7, .7), 
                                    Point(18, 55.8, 10),
                                    Point(18, 55.8, 18),
                                    Point(10, 55.8, 18),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al3);
    scene.numLights++;
    AreaLight *al4 = new AreaLight(RGB(0.7, 0.7, 0.7), 
                                    Point(18, 55.8, 10),
                                    Point(10, 55.8, 18),
                                    Point(10, 55.8, 10),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al4);
    scene.numLights++;
    
    AreaLight *al5 = new AreaLight(RGB(1, 1, 1), 
                                    Point(-18, 55.8, 10),
                                    Point(-18, 55.8, 18),
                                    Point(-10, 55.8, 18),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al5);
    scene.numLights++;
    AreaLight *al6 = new AreaLight(RGB(1, 1, 1), 
                                    Point(-18, 55.8, 10),
                                    Point(-10, 55.8, 18),
                                    Point(-10, 55.8, 10),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al6);
    scene.numLights++;

    AreaLight *al7 = new AreaLight(RGB(1, 1, 1), 
                                    Point(-18, 27.9, 10),
                                    Point(-18, 27.9, 18),
                                    Point(-10, 27.9, 18),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al7);
    scene.numLights++;
    AreaLight *al8 = new AreaLight(RGB(1, 1, 1), 
                                    Point(-18, 27.9, 10),
                                    Point(-10, 27.9, 18),
                                    Point(-10, 27.9, 10),
                                    Vector(0, -1, 0));
    scene.lights.push_back(al8);
    scene.numLights++;

    scene.printSummary();
    std::cout << std::endl;
    // scene.printScene();

    // Image resolution
    const int W= 512;
    const int H= 512;
    
    img = new ImagePPM(W,H);
    
    // Camera parameters
    // const Point Eye ={280,275,-330}, At={280,265,0};
    // const Point Eye ={25, 20, 0}, At={0,0,0};
    const Point Eye ={0, 25, -25}, At={0,25,0};
    const Vector Up={0,1,0};
    // const float fovW = 60.f;
    const float fovW = 90.f;
    const float fovH = fovW * (float)H/(float)W;  // in degrees
    const float fovWrad = fovW*3.14f/180.f, fovHrad = fovH*3.14f/180.f;    // to radians
    cam = new Perspective(Eye, At, Up, W, H, fovWrad, fovHrad);
    // create the shader
    RGB background(0.05, 0.05, 0.55);
    // shd = new AmbientShader(&scene, background);
    // shd = new WhittedShader(&scene, background);
    // shd = new DistributedShader(&scene, background);
    shd = new PathTracerShader(&scene, background); 
    // declare the renderer
    int spp=8;     // samples per pixel
    // StandardRenderer myRender (cam, &scene, img, shd, spp);
    WindowRenderer myRender (cam, &scene, img, shd, spp);
    // render
    start = clock();
    myRender.Render();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // save the image
    img->Save("MyImage.ppm");
    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}