cmake_minimum_required(VERSION 4.1)
project(3D_render)

include_directories(src)

include_directories(libraries/SDL/include/SDL3)
add_subdirectory(libraries/SDL EXCLUDE_FROM_ALL)

#set(SDL3_DIR "${CMAKE_SOURCE_DIR}/libraries/SDL/cmake-build-debug")
#find_package(SDL3 REQUIRED)
include_directories(libraries/SDL_image/include/SDL3_image)
add_subdirectory(libraries/SDL_image EXCLUDE_FROM_ALL)

include_directories(libraries/imgui)
add_subdirectory(libraries/imgui EXCLUDE_FROM_ALL)

include_directories(libraries/gltf/include)
add_subdirectory(libraries/gltf EXCLUDE_FROM_ALL)

set(CMAKE_CXX_STANDARD 20)

add_executable(3D_render
        src/main.cpp
        src/camera.cpp
        src/material.cpp
        src/object.cpp
        src/vertice.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui
    fastgltf
)