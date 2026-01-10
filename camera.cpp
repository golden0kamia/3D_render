#include "camera.hpp"

camera::camera(SDL_Renderer* renderer)
{
	this->renderer = renderer;
	this->fov = 90;
	this->set_screen_size({ 640, 480 });
}

int camera::render_object(std::vector<object> objects)
{
	//this->normal = { 0.f, 0.f, -1.f };
	vertice_3_f camera_v1_rot = rot_xyz({ 0.f, 0.f, 0.f }, this->angle);
	vertice_3_f camera_v2_rot = rot_xyz({ 0.f, -1.f, 0.f }, this->angle);
	vertice_3_f camera_v3_rot = rot_xyz({ 1.f, 0.f, 0.f }, this->angle);
	vertice_3_f camera_vec1 = diff_vertice(camera_v1_rot, camera_v2_rot);
	vertice_3_f camera_vec2 = diff_vertice(camera_v1_rot, camera_v3_rot);
	this->normal = cross_product(camera_vec1, camera_vec2);

	std::vector<vertex_gemoetry> trigon;
	float length = this->length();
	for (object& obj : objects) {
		if (!obj.display) continue;
		for (const face_f& face : obj.faces) {
			// Get vertices
			vertice_3_f v1 = face.v1;
			vertice_3_f v2 = face.v2;
			vertice_3_f v3 = face.v3;

			// Object size
			vertice_3_f v1_size = v1 * obj.size;
			vertice_3_f v2_size = v2 * obj.size;
			vertice_3_f v3_size = v3 * obj.size;

			// Object local rotation
			vertice_3_f v1_rot = rot_xyz(v1_size, obj.angle);
			vertice_3_f v2_rot = rot_xyz(v2_size, obj.angle);
			vertice_3_f v3_rot = rot_xyz(v3_size, obj.angle);

			// Object global position with camera
			vertice_3_f v1_pos = v1_rot + obj.position - this->position;
			vertice_3_f v2_pos = v2_rot + obj.position - this->position;
			vertice_3_f v3_pos = v3_rot + obj.position - this->position;

			// Object with camera rotation
			vertice_3_f v1_rot_cam = rot_xyz(v1_pos, this->angle.negate());
			vertice_3_f v2_rot_cam = rot_xyz(v2_pos, this->angle.negate());
			vertice_3_f v3_rot_cam = rot_xyz(v3_pos, this->angle.negate());
			if (v1_rot_cam.z < 0 && v2_rot_cam.z < 0 && v3_rot_cam.z < 0) continue;

			vertice_3_f vec1 = diff_vertice(v1_rot_cam, v2_rot_cam);
			vertice_3_f vec2 = diff_vertice(v1_rot_cam, v3_rot_cam);
			vertice_3_f face_normal = cross_product(vec1, vec2);
			float normale = -dot_product(this->normal, face_normal);
			float normalise_normal = std::copysignf((normale * normale) / (face_normal.x * face_normal.x + face_normal.y * face_normal.y + face_normal.z * face_normal.z), normale);
			if (normalise_normal < 0.f) continue;

			// 2D paralax
			SDL_FPoint v1_abs = { ((v1_rot_cam.x) / (v1_rot_cam.z)) * length , // (O.x/O.z)*c.L
								  ((v1_rot_cam.y) / (v1_rot_cam.z)) * length };// (O.y/O.z)*c.L
			SDL_FPoint v2_abs = { ((v2_rot_cam.x) / (v2_rot_cam.z)) * length ,
								  ((v2_rot_cam.y) / (v2_rot_cam.z)) * length };
			SDL_FPoint v3_abs = { ((v3_rot_cam.x) / (v3_rot_cam.z)) * length ,
								  ((v3_rot_cam.y) / (v3_rot_cam.z)) * length };

			// Object  center to screen
			SDL_FPoint v1_2D = { v1_abs.x + this->center.x ,
								 v1_abs.y + this->center.y };
			SDL_FPoint v2_2D = { v2_abs.x + this->center.x ,
								 v2_abs.y + this->center.y };
			SDL_FPoint v3_2D = { v3_abs.x + this->center.x ,
								 v3_abs.y + this->center.y };

			// Create white color
			//SDL_Color face_color = color_factor(obj.color, sin(normalise_normal * (M_PI / 2)));
			SDL_Color face_color = color_factor(face.mtl->Kd, std::max(normalise_normal, this->global_light));

			// Create vertex for geometry
			SDL_Vertex vx1 = { v1_2D, face_color, face.vt1 };
			SDL_Vertex vx2 = { v2_2D, face_color, face.vt2 };
			SDL_Vertex vx3 = { v3_2D, face_color, face.vt3 };

			// Create list of vertex to display
			if (face.mtl == nullptr) {
				SDL_Log("Error material empty");
			}
			vertex_gemoetry vxs = { vx1, vx2, vx3 , (v1_pos.z + v2_pos.z + v3_pos.z) / 3.f, face.mtl };
			trigon.push_back(vxs);

		}
	}
	std::sort(trigon.begin(), trigon.end(), [](vertex_gemoetry& f1, vertex_gemoetry& f2) {
		return f1.z > f2.z;
		});
	
	for (vertex_gemoetry face : trigon) {
		SDL_Vertex vxs[] = { face.v1, face.v2, face.v3 };
		SDL_SetRenderDrawColor(this->renderer, face.mtl->Kd.r, face.mtl->Kd.g, face.mtl->Kd.b, face.mtl->d);
		SDL_RenderGeometry(this->renderer, face.mtl->map_Kd, vxs, 3, nullptr, 0);

		// Rendering mesh
		if (this->mesh_display) {
			SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
			SDL_RenderDrawLineF(this->renderer, face.v1.position.x, face.v1.position.y, face.v2.position.x, face.v2.position.y);
			SDL_RenderDrawLineF(this->renderer, face.v2.position.x, face.v2.position.y, face.v3.position.x, face.v3.position.y);
			SDL_RenderDrawLineF(this->renderer, face.v3.position.x, face.v3.position.y, face.v1.position.x, face.v1.position.y);
		}
	}
	if (trigon.empty()) return 0;
	// Display texture for control
	//SDL_Rect dst = { 0, 0, 20, 20 };
	//SDL_RenderCopy(this->renderer, trigon[0].texture, nullptr, &dst);
	return 0;
}

void camera::set_screen_size(SDL_Point screen_size)
{
	this->screen_size = screen_size;
	this->center = { screen_size.x / 2.f, screen_size.y / 2.f };
}

float camera::length()
{
	return (this->screen_size.x / 2) / tanf(M_PI*(fov/360.f));
}

SDL_Color color_factor(SDL_Color color, float factor)
{
	return { (Uint8)(color.r * factor), (Uint8)(color.g * factor), (Uint8)(color.b * factor), color.a };
}
