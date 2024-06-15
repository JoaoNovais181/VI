//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <AmbientShader.hpp>
#include <ImagePPM.hpp>

const bool jitter = true;

void StandardRenderer::Render()
{
    int W = 0, H = 0; // resolution
    int x, y, ss;

    // get resolution from the camera
    cam->getResolution(&W, &H);

    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            Ray primary;
            Intersection isect;
            bool intersected;
            RGB color = RGB(0,0,0);

            for (ss = 0 ; ss < spp ; ss++)
            {

                // Generate Ray (camera)
                if (jitter) {
                    float jitterV[2];
                    jitterV[0] = ((float)rand()) / ((float)RAND_MAX);
                    jitterV[1] = ((float)rand()) / ((float)RAND_MAX);
                    cam->GenerateRay(x, y, &primary, jitterV);
                } else {
                    cam->GenerateRay(x, y, &primary);
                }
                // trace ray (scene)
                intersected = scene->trace(primary, &isect);
                
                // shade this intersection (shader) - remember: depth=0
                color += shd->shade(intersected, isect, 0);
            }
            color = color / spp;
            // write the result into the image frame buffer (image)
            img->set(x,y,color);
            
        } // loop over columns
    }   // loop over rows

    // Image *soma = new Image(W, H);

    // // main rendering loop: get primary rays from the camera until done
    // for (ss = 0; ss < spp; ss++)
    // { // supersampling loop
    //     for (y = 0; y < H; y++)
    //     { // loop over rows
    //         for (x = 0; x < W; x++)
    //         { // loop over columns
    //             Ray primary;
    //             Intersection isect;
    //             bool intersected;
    //             RGB color = RGB(0, 0, 0);

    //             // Generate Ray (camera)
    //             if (jitter)
    //             {
    //                 float jitterV[2];
    //                 jitterV[0] = ((float)rand()) / ((float)RAND_MAX);
    //                 jitterV[1] = ((float)rand()) / ((float)RAND_MAX);
    //                 cam->GenerateRay(x, y, &primary, jitterV);
    //             }
    //             else
    //             {
    //                 cam->GenerateRay(x, y, &primary);
    //             }

    //             // trace ray (scene)
    //             intersected = scene->trace(primary, &isect);

    //             // shade this intersection (shader) - remember: depth=0
    //             color = shd->shade(intersected, isect, 0);

    //             // accumulate color into the image frame buffer (image)
    //             if (ss == 0)
    //                 soma->set(x,y, color);
    //             else
    //                 soma->add(x,y, color);

    //             img->set(x, y, soma->get(x,y) / ((float)ss+1));
                
    //         } // loop over columns
    //     } // loop over rows
    //     char name[64];

    //     sprintf(name, "Standard_%d.ppm", ss+1);
    //     ((ImagePPM*)img)->Save(name);
    //     printf("OI\n");
    // } // supersampling loop

    // // average the color for each pixel
    // for (y = 0; y < H; y++)
    // {
    //     for (x = 0; x < W; x++)
    //     {
    //         RGB color = soma->get(x, y);
    //         color = color / spp;
    //         img->set(x, y, color);
    //     }
    // }
}
