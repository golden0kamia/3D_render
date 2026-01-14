#pragma once
#include "object.hpp"
#include <algorithm>

class camera :
    public object
{
public:
    int fov = 90;
    SDL_Renderer* renderer = nullptr;
    SDL_Point screen_size = { 640, 480 };
    SDL_FPoint center = { 320, 240 };
    bool mesh_display = false;
    //float length = 10;
    float global_light = 0.25f;
    vertice_3_f normal;

    camera(SDL_Renderer* renderer);

    int render_object(std::vector<object> objects);
    void set_screen_size(SDL_Point screen_size);
    float length();
};

SDL_Color color_factor(SDL_Color color, float factor);

typedef struct vertex_gemoetry {
    SDL_Vertex v1;
    SDL_Vertex v2;
    SDL_Vertex v3;
    float z;
    material* mtl;
}vertex_gemoetry;

