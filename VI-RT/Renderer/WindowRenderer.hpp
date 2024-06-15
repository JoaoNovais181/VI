#ifndef WINDOW_RENDERER_H
#define WINDOW_RENDERER_H


#include "renderer.hpp"

alignas(16) struct Vec3
{
    float r, g, b;

    Vec3() : r(0), g(0), b(0) {}
    Vec3(float r, float g, float b) : r(r), g(g), b(b) {}

    // Optional: Add utility functions if needed
    Vec3 operator+(const Vec3 &other) const
    {
        return Vec3(r + other.r, g + other.g, b + other.b);
    }

    Vec3 &operator+=(const Vec3 &other)
    {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Vec3 operator*(float scalar) const
    {
        return Vec3(r * scalar, g * scalar, b * scalar);
    }

    Vec3 &operator*=(float scalar)
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }
};

class GLFWwindow;

class WindowRenderer: public Renderer {
private:
    unsigned int texture, program;
    GLFWwindow *window;
    int W,H;
    bool running;
    RGB average;
    void initializeTexture();
    void updateTexture();
    std::vector<float> colorData;
    

public:
    int spp;
    WindowRenderer (Camera *cam, Scene * scene, Image * img, Shader *shd, int _spp): Renderer(cam, scene, img, shd) {
        spp = _spp;
    }
    void Render ();
    void calculateBuffers();
};


#endif