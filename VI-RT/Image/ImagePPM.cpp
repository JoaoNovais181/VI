//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

void ImagePPM::Uncharted2ToneMap()
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    const float exposureBias = 2.0f;
    const float invGamma = 1/2.2f;

    imageToSave = new PPM_pixel[W * H];

    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; ++i)
        {
            RGB x = imagePlane[j * W + i] * 2.0f;
            RGB saveColor = ((x * (x * A + RGB(C * B)) + RGB(D * E)) / (x * (x * A + RGB(B)) + RGB(D * F))) - RGB(E / F);
            saveColor = saveColor * exposureBias;
            saveColor.R = powf(saveColor.R, invGamma);
            saveColor.G = powf(saveColor.G, invGamma);
            saveColor.B = powf(saveColor.B, invGamma);
            imageToSave[j * W + i].val[0] = (unsigned char)(saveColor.R * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(saveColor.G * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(saveColor.B * 255);
        }
    }
}

void ImagePPM::ToneMap()
{
    imageToSave = new PPM_pixel[W * H];

    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; ++i)
        {
            imageToSave[j * W + i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].R) * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].G) * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].B) * 255);
        }
    }
}

bool ImagePPM::Save(std::string filename)
{

    // convert from float to {0,1,..., 255}
    ToneMap();
    // Uncharted2ToneMap();

    // write imageToSave to file
    if (W == 0 || H == 0)
    {
        fprintf(stderr, "Can't save an empty image\n");
        return false;
    }

    std::ofstream ofs;
    try
    {
        ofs.open(filename, std::ios::binary); // need to spec. binary mode for Windows users
        if (ofs.fail())
            throw("Can't open output file");
        ofs << "P6\n"
            << W << " " << H << "\n255\n";
        unsigned char r, g, b;
        // loop over each pixel in the image, clamp and convert to byte format
        for (int i = 0; i < W * H; ++i)
        {
            PPM_pixel pix = imageToSave[i];
            ofs << pix.val[0] << pix.val[1] << pix.val[2];
        }
        ofs.close();
    }
    catch (const char *err)
    {
        fprintf(stderr, "%s\n", err);
        ofs.close();
    }

    // Details and code on PPM files available at:
    // https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html

    return true;
}
