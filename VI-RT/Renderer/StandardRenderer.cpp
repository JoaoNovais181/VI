//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <AmbientShader.hpp>

const bool jitter = true;

void StandardRenderer::Render()
{
    int W = 0, H = 0; // resolution
    int x, y, ss;

    // get resolution from the camera
    cam->getResolution(&W, &H);

    // main rendering loop: get primary rays from the camera until done
    for (ss = 0; ss < spp; ss++)
    {
        for (y = 0; y < H; y++)
        { // loop over rows
            Ray primary;
            Intersection isect;
            bool intersected;
            RGB color = RGB(0, 0, 0);

            for (x = 0; x < W; x++)
            { // loop over columns
                // Generate Ray (camera)
                if (jitter)
                {
                    float jitterV[2];
                    jitterV[0] = ((float)rand()) / ((float)RAND_MAX);
                    jitterV[1] = ((float)rand()) / ((float)RAND_MAX);
                    cam->GenerateRay(x, y, &primary, jitterV);
                }
                else
                {
                    cam->GenerateRay(x, y, &primary);
                }
                // trace ray (scene)
                isect.depth = MAXFLOAT;
                intersected = scene->trace(primary, &isect);

                // printf("\nPonto %f %f %f\n", isect.p.X, isect.p.Y, isect.p.Z);
                // printf("Depth %f\n", isect.depth);

                // shade this intersection (shader) - remember: depth=0
                color = shd->shade(intersected, isect, 0);
                img->add(x, y, color);
            }
        } // loop over columns
    } // loop over rows

    for (y = 0; y < H; y++)
        for (x = 0; x < W; x++)
            img->divide(x, y, spp);
}
