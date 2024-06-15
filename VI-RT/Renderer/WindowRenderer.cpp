#include "WindowRenderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <mutex>

#include "linmath.h"
#include "ImagePPM.hpp"
#include "perspective.hpp"

const bool jitter = true;

Perspective *global_cam;
Image *global_img;
bool moved = false;

float min(float a, float b)
{
    return (a < b) ? a : b;
}

/**
 * TODO: adicionar buffers para os shaders
 *
 */
void WindowRenderer::calculateBuffers()
{
    printf("Thread comecou\n");

    int W = 0, H = 0; // resolution
    int x, y;


    cam->getResolution(&W, &H);

    Image *localImg = new Image(W, H);
    RGB localAverage;
    localAverage = RGB(0, 0, 0);

    // main rendering loop: get primary rays from the camera until done
    for (spp = 1; running; spp++)
    {
        char buffer[128];
        snprintf(buffer, 128, "Ray Tracer - %d spp", spp);
        glfwSetWindowTitle(window, buffer);

        localAverage = RGB(0,0,0);


        for (y = 0; y < H && !moved; y++)
        { // loop over rows

            for (x = 0; x < W && !moved; x++)
            { // loop over columns
                // Generate Ray (camera)
                Ray primary;
                Intersection isect;
                bool intersected;
                RGB color = RGB(0, 0, 0);
                
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
                intersected = scene->trace(primary, &isect);

                // shade this intersection (shader) - remember: depth=0
                color = shd->shade(intersected, isect, 0);

                // if (x==400 && y==300)
                //     printf("Color (0,0): %f %f %f\n\n", color.R, color.G, color.B);


                localAverage += color;

                // accumulate color
                localImg->add(x, y, color);

                // update color data for texture
                RGB avgColor = localImg->get(x, y) / (float)spp;
                img->set(x, y, avgColor);
                colorData[(y * W + x) * 3 + 0] = avgColor.R;
                colorData[(y * W + x) * 3 + 1] = avgColor.G;
                colorData[(y * W + x) * 3 + 2] = avgColor.B;
            }
        } // loop over columns
        average = (average * (spp - 1) + (localAverage / (H * W))) / spp;
        if (moved)
        {
            img->reset();
            localImg->reset();
            localAverage = RGB(0, 0, 0);
            average = RGB(0, 0, 0);
            spp = 0;
        }
        moved = false;
    } // loop over rows
}

// Initialize texture with vec3 color data
void WindowRenderer::initializeTexture()
{
    colorData = std::vector<float>(W * H * 3); // Initialize with black colors

    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            colorData[y * W + x + 0] = 1;
            colorData[y * W + x + 1] = 1;
            colorData[y * W + x + 2] = 1;
        }
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGB, GL_FLOAT, colorData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Update the color at specific (x, y)
void WindowRenderer::updateTexture()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB, GL_FLOAT, colorData.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

static const char *vertex_shader_text =
    "#version 460\n"
    "uniform mat4 MVP;\n"
    "uniform vec2 size;\n"
    "uniform vec2 imgSize;\n"
    "in vec2 vPos;\n"
    "out vec2 pos;\n"
    "void main()\n"
    "{\n"
    "    vec2 ndcPos = vec2(\n"
    "        2.0 * (vPos.x / imgSize.x) - 1.0,\n"
    "        2.0 * (vPos.y / imgSize.y) - 1.0\n"
    "    );\n"
    "    pos = vPos;\n"
    "    gl_Position = vec4(ndcPos, 0.0, 1.0);\n"
    "}\n";

static const char *fragment_shader_text =
    // "#version 460\n"
    // "uniform sampler2D colorTexture;\n"
    // "uniform vec2 size;\n"
    // "uniform vec2 imgSize;\n"
    // "uniform vec3 average;\n"
    // "in vec2 pos;\n"
    // "out vec4 color;\n"

    // "vec3 Uncharted2Tonemap(vec3 x)\n"
    // "{\n"
    // "    const float A = 0.15;\n"
    // "    const float B = 0.50;\n"
    // "    const float C = 0.10;\n"
    // "    const float D = 0.20;\n"
    // "    const float E = 0.02;\n"
    // "    const float F = 0.30;\n"
    // "    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;\n"
    // "}\n"

    // "void main()\n"
    // "{\n"
    // "    vec2 texCoord = vec2(pos.x / imgSize.x, 1.0 - pos.y / imgSize.y);\n"
    // "    vec3 hdrColor = texture(colorTexture, texCoord).rgb;\n"
    // "    \n"
    // "    // Uncharted 2 tone mapping\n"
    // "    vec3 toneMappedColor = Uncharted2Tonemap(hdrColor * 2.0); // Scale factor can be adjusted\n"
    // "    \n"
    // "    // Exposure bias\n"
    // "    float exposureBias = 2.0;\n"
    // "    toneMappedColor = toneMappedColor * exposureBias;\n"
    // "    \n"
    // "    // Gamma correction\n"
    // "    const float gamma = 2.2;\n"
    // "    vec3 gammaCorrected = pow(toneMappedColor, vec3(1.0 / gamma));\n"
    // "    \n"
    // "    color = vec4(gammaCorrected, 1.0);\n"
    // "}\n";
    "#version 460\n"
    "uniform sampler2D colorTexture;\n"
    "uniform vec2 size;\n"
    "uniform vec2 imgSize;\n"
    "uniform vec3 average;\n"
    "in vec2 pos;\n"
    "out vec4 color;\n"

    "void main()\n"
    "{\n"
    "    vec2 texCoord = vec2(pos.x / imgSize.x, 1.0 - pos.y / imgSize.y);\n"
    "    vec3 hdrColor = texture(colorTexture, texCoord).rgb;\n"
    "    \n"
    "    hdrColor.r = min(1.0, hdrColor.r);"
    "    hdrColor.g = min(1.0, hdrColor.g);"
    "    hdrColor.b = min(1.0, hdrColor.b);"
    "    \n"
    "    color = vec4(hdrColor, 1.0);\n"
    "}\n";

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_D || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_W)
    {
        Point eye = global_cam->getEye(), at = global_cam->getAt();
        Vector Up = global_cam->getUp();
        Vector lookDir = at - eye;
        Vector right = lookDir.cross(Up);

        Up.normalize();
        lookDir.normalize();
        right.normalize();

        if (key == GLFW_KEY_D)
            global_cam->moveFPS(right);
        else if (key == GLFW_KEY_A)
            global_cam->moveFPS(right * -1.0f);
        else if (key == GLFW_KEY_W)
            global_cam->moveFPS(lookDir);
        else if (key == GLFW_KEY_S)
            global_cam->moveFPS(lookDir * -1.0f);
        moved = true;
    }
    else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
    {
        Point eye = global_cam->getEye();
        Vector lookDir = global_cam->getAt() - eye;
        Vector right = lookDir.cross(global_cam->getUp());
        Vector Up = global_cam->getUp();

        lookDir.normalize();
        right.normalize();
        Up.normalize();

        float angle = 2.5f * (M_PI / 180.0f); // 2.5 degrees in radians

        Vector rotatedLookDir = lookDir;
        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            rotatedLookDir.Y = lookDir.Y * cos(angle) - lookDir.Z * sin(angle);
            rotatedLookDir.Z = lookDir.Y * sin(angle) + lookDir.Z * cos(angle);
            global_cam->setAt(eye + rotatedLookDir);
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            angle *= -1;
            rotatedLookDir.Y = lookDir.Y * cos(angle) - lookDir.Z * sin(angle);
            rotatedLookDir.Z = lookDir.Y * sin(angle) + lookDir.Z * cos(angle);
            global_cam->setAt(eye + rotatedLookDir);
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            rotatedLookDir.X = lookDir.X * cos(angle) + lookDir.Z * sin(angle);
            rotatedLookDir.Z = -lookDir.X * sin(angle) + lookDir.Z * cos(angle);
            global_cam->setAt(eye + rotatedLookDir);
        }
        else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            angle *= -1;
            rotatedLookDir.X = lookDir.X * cos(angle) + lookDir.Z * sin(angle);
            rotatedLookDir.Z = -lookDir.X * sin(angle) + lookDir.Z * cos(angle);
        }
        global_cam->setAt(eye + rotatedLookDir);
        moved = true;
    }
}

bool checkCompilation(GLint shader, std::string shaderName) {
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        printf("%s:\n%s\n", shaderName.c_str(), errorLog.data());

        glDeleteShader(shader); // Don't leak the shader.
        return true;
    }
    return false;
}

void WindowRenderer::Render()
{
    global_cam = static_cast<Perspective *>(cam);
    global_img = img;
    int x, y, ss;

    // get resolution from the camera
    cam->getResolution(&W, &H);

    static const struct
    {
        float x, y;
    } vertices[6] =
        {
            {0.0f, 0.0f},
            {(float)W, 0.0f},
            {0.0f, (float)H},
            {0.0f, (float)H},
            {(float)W, 0.0f},
            {(float)W, (float)H}};

    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, size_location, vpos_location, average_location, imgSize_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(W, H, "Ray Tracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        return;
    }

    initializeTexture();

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    if (checkCompilation(fragment_shader, "Fragment Shader") ||
        checkCompilation(vertex_shader, "Vertex Shader"))
        return;
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    size_location = glGetUniformLocation(program, "size");
    vpos_location = glGetAttribLocation(program, "vPos");
    average_location = glGetUniformLocation(program, "average");
    imgSize_location = glGetUniformLocation(program, "imgSize");
    
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);

    // color calculation thread
    auto f = [](WindowRenderer *r)
    {
        r->calculateBuffers();
    };

    std::thread thread_object(f, this);

    running = true;
    while (!glfwWindowShouldClose(window))
    {
        this->updateTexture();
        glUniform3f(average_location, average.R, average.G, average.B);
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        // glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
        glUniform2f(imgSize_location, W, H);
        glUniform2f(size_location, width, height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Render your scene
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    running = false;
    thread_object.join();

    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);

    glfwTerminate();
}
