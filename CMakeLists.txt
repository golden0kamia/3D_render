cmake_minimum_required(VERSION 4.1)
project(3D_render)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(libraries/imgui)
include_directories(libraries/imgui/backends)
include_directories(libraries/SDL/include)
include_directories(libraries/SDL_image/include)

add_executable(3D_render
        src/camera.cpp
        src/camera.hpp
        src/face.hpp
        src/main.cpp
        src/main.hpp
        src/material.cpp
        src/material.hpp
        src/object.cpp
        src/object.hpp
        src/vertice.cpp
        src/vertice.hpp)
