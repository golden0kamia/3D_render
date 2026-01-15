/*****************************************************************//**
 * \file   vertice.hpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/

#ifndef VERTICE_HPP
#define VERTICE_HPP

#include <cmath>
#include <SDL3/SDL.h>

typedef struct vertice_3_f {
	float x, y, z;

	vertice_3_f operator+(const vertice_3_f& vertice) {
		return { this->x + vertice.x, 
				 this->y + vertice.y,
				 this->z + vertice.z };
	}
	vertice_3_f operator-(const vertice_3_f& vertice) {
		return { this->x - vertice.x,
				 this->y - vertice.y,
				 this->z - vertice.z };
	}
	vertice_3_f operator*(const vertice_3_f& vertice) {
		return { this->x * vertice.x,
				 this->y * vertice.y,
				 this->z * vertice.z };
	}
	vertice_3_f negate() {
		return { -x,
				 -y,
				 -z };
	}
}vertice_3_f;

typedef struct vertice_3_i {
	long x, y, z;
}vertice_3_i;

typedef struct edge_f {
	vertice_3_f p1, p2;
}edge_f;

typedef struct edge_i {
	vertice_3_i p1, p2;
}edge_i;

vertice_3_f rot_x(vertice_3_f vertice, float alpha);
vertice_3_f rot_y(vertice_3_f vertice, float alpha);
vertice_3_f rot_z(vertice_3_f vertice, float alpha);
vertice_3_f rot_xyz(vertice_3_f vertice, vertice_3_f rotation);

float dot_product(vertice_3_f v1, vertice_3_f v2);
vertice_3_f cross_product(vertice_3_f v1, vertice_3_f v2);
vertice_3_f diff_vertice(vertice_3_f v1, vertice_3_f v2);

#endif
