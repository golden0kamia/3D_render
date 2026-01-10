#pragma once
#include <SDL.h>
#include <string>

typedef struct color_3_f {
	float r;
	float g;
	float b;
}color_3_f;

class material
{
public:
	float Ns; // Specular exponant
	SDL_Color Ka = { 255, 255, 255, 255 }; // Ambiant color
	SDL_Color Kd = { 255, 255, 255, 255 }; // Diffuse color
	SDL_Color Ks = { 255, 255, 255, 255 }; // Specular color
	SDL_Color Ke = { 255, 255, 255, 255 }; // Emission color
	float Ni; // Index of refraction
	float d; // Dissolved ( 1/transparency )
	float Tr; // Transparency ( 1/Dissolved )
	int illum; // Illumination mode
	SDL_Texture* map_Ka; // Ambiant texture map
	SDL_Texture* map_Kd; // Diffuse texture map
	SDL_Texture* map_Ks; // Specular texture map
	SDL_Texture* map_Ns; // Specular highlight map
	SDL_Texture* map_d; // Alpha texture map
	SDL_Texture* map_bump;
	SDL_Texture* disp; // Displacement map
	SDL_Texture* decal;
	std::string name;
};

