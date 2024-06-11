cmake_minimum_required(VERSION 3.12)

project(VI_RT)

set(CMAKE_BUILD_TYPE DEBUG)

file(GLOB 3DSortingStruct_SRC 
            "3DSortingStruct/*.hpp"
            "3DSortingStruct/*.cpp")

file(GLOB Camera_SRC
            "Camera/*hpp"
            "Camera/*.cpp")

file(GLOB Image_SRC
            "Image/*.hpp"
            "Image/*.cpp")

file(GLOB Image_SRC
            "Image/*.hpp"
            "Image/*.cpp")

file(GLOB Light_SRC
            "Light/*.hpp"
            "Light/*.cpp")

file(GLOB Primitive_SRC
            "Primitive/*.hpp"
            "Primitive/BRDF/*.hpp"
            "Primitive/Geometry/*.hpp"
            "Primitive/*.cpp"
            "Primitive/BRDF/*.cpp"
            "Primitive/Geometry/*.cpp")

file(GLOB Renderer_SRC
            "Renderer/*.hpp"
            "Renderer/*.cpp")

file(GLOB Scene_SRC
            "Scene/*.hpp"
            "Scene/*.cpp")

file(GLOB Shader_SRC
            "Shader/*.hpp"
            "Shader/*.cpp")

file(GLOB Utils_SRC
            "Utils/*.hpp"
            "Utils/*.cpp")

add_subdirectory(Scene/tinyobjloader)
list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/tinyobjloader")