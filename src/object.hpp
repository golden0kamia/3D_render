/*****************************************************************//**
 * \file   object.hpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/
#pragma once
#include <face.hpp>
#include <material.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <string>
#include <fstream>
#include <strstream>
#include <vector>
#include <fastgltf/core.hpp>

class object
{
public:
	vertice_3_f position = { 0., 0., 0. };
	vertice_3_f angle = { 0., 0., 0. };
	vertice_3_f size = { 1., 1., 1. };
	std::vector<vertice_3_f> vertices;
	std::vector<SDL_FPoint> vertices_texture;
	std::vector<vertice_3_f> vertices_normal;
	std::vector<face_f> faces;
	std::string name;
	bool display = true;

	object();
	object(std::string filepath);
	int load(std::string filepath);

};

