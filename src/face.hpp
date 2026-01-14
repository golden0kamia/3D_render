/*****************************************************************//**
 * \file   face.hpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/

#pragma once
#ifndef FACE_HPP
#define FACE_HPP

#include "vertice.hpp"
#include "material.hpp"

typedef struct face_f {
	vertice_3_f v1, v2, v3;
	SDL_FPoint vt1, vt2, vt3;
	vertice_3_f vn1, vn2, vn3;
	material* mtl;
} face_f;

#endif // !FACE_HPP


