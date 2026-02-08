#include "camera.hpp"

#include <complex>

camera::camera(SDL_Renderer* renderer)
{
	this->renderer = renderer;
	this->fov = 90;
	this->set_screen_size({ 640, 480 });
}

int camera::render_object(std::vector<object> objects)
{
	// Calculate the normal vector of camera view
	vertice_3_f camera_v1_rot = rot_xyz({ 0.f, 0.f, 0.f }, this->angle);
	vertice_3_f camera_v2_rot = rot_xyz({ 0.f, -1.f, 0.f }, this->angle);
	vertice_3_f camera_v3_rot = rot_xyz({ 1.f, 0.f, 0.f }, this->angle);
	vertice_3_f camera_vec1 = diff_vertice(camera_v1_rot, camera_v2_rot);
	vertice_3_f camera_vec2 = diff_vertice(camera_v1_rot, camera_v3_rot);
	this->normal = cross_product(camera_vec1, camera_vec2);
	// FOV angulare normal
	camera_v1_rot = rot_xyz({ 0.f, 0.f, 0.f }, {static_cast<float>(M_PI)/2-this->fov/2, 0.f, 0.f});
	camera_v2_rot = rot_xyz({ 0.f, -1.f, 0.f }, {static_cast<float>(M_PI)/2-this->fov/2, 0.f, 0.f});
	camera_v3_rot = rot_xyz({ 1.f, 0.f, 0.f }, {static_cast<float>(M_PI)/2-this->fov/2, 0.f, 0.f});
	camera_vec1 = diff_vertice(camera_v1_rot, camera_v2_rot);
	camera_vec2 = diff_vertice(camera_v1_rot, camera_v3_rot);
	vertice_3_f fov_normal = cross_product(camera_vec1, camera_vec2);
	float view_normal_correlation = 0.00258;//1-dot_product(this->normal, fov_normal);

	std::vector<vertex_geometry> trigon;
	float length = this->length();
	// Compute each object of list
	for (object& obj : objects) {
		// Stop compute if object is not enable for display
		if (!obj.display) continue;
		// Compute each face of object
		for (const face_f& face : obj.faces) {
			// Get vertices of face
			vertice_3_f v1 = face.vx;
			vertice_3_f v2 = face.vy;
			vertice_3_f v3 = face.vz;

			// Apply object size of face
			vertice_3_f v1_size = v1 * obj.size;
			vertice_3_f v2_size = v2 * obj.size;
			vertice_3_f v3_size = v3 * obj.size;

			// Apply object local rotation of face
			vertice_3_f v1_rot = rot_xyz(v1_size, obj.angle);
			vertice_3_f v2_rot = rot_xyz(v2_size, obj.angle);
			vertice_3_f v3_rot = rot_xyz(v3_size, obj.angle);

			// Apply object global position with camera
			vertice_3_f v1_pos = v1_rot + obj.position - this->position;
			vertice_3_f v2_pos = v2_rot + obj.position - this->position;
			vertice_3_f v3_pos = v3_rot + obj.position - this->position;

			// Apply object with camera rotation
			vertice_3_f v1_rot_cam = rot_xyz(v1_pos, this->angle.negate());
			vertice_3_f v2_rot_cam = rot_xyz(v2_pos, this->angle.negate());
			vertice_3_f v3_rot_cam = rot_xyz(v3_pos, this->angle.negate());
			if (v1_rot_cam.z < 0 && v2_rot_cam.z < 0 && v3_rot_cam.z < 0) continue;

			// Create normal vector from face vertices
			vertice_3_f vec1 = diff_vertice(v1_rot_cam, v2_rot_cam); // vertice.1-vertice.2
			vertice_3_f vec2 = diff_vertice(v1_rot_cam, v3_rot_cam); // vertice.1-vertice.3
			vertice_3_f face_normal = cross_product(vec1, vec2); // vector.1 cross vector.2

			vertice_3_f face_centroid = centroid(v1_rot_cam, v2_rot_cam, v3_rot_cam); // Get face center of mass
			float face_center_hypo = std::sqrt(face_centroid.x * face_centroid.x + face_centroid.y * face_centroid.y); // Get face xy hypothenus
			float max_normal = abs(sin(std::atan(face_center_hypo/face_centroid.z)))/face_centroid.z; // Get face relative normal ratio from position

			// Calculate relative normale correlation
			float normale_correlation = -dot_product(this->normal, face_normal); // camera.normal dot face.normal
			float normalise_normal = std::copysignf((normale_correlation * normale_correlation) / (face_normal.x * face_normal.x + face_normal.y * face_normal.y + face_normal.z * face_normal.z), normale_correlation); //
			if (normalise_normal < max_normal) continue; // If face do not face camera, do not render

			// Create 2D positon from 3D parallax
			SDL_FPoint v1_abs = { ((v1_rot_cam.x) / (v1_rot_cam.z)) * length , // (face.x/face.z)*camera.L
								  ((v1_rot_cam.y) / (v1_rot_cam.z)) * length };// (face.y/face.z)*camera.L
			SDL_FPoint v2_abs = { ((v2_rot_cam.x) / (v2_rot_cam.z)) * length ,
								  ((v2_rot_cam.y) / (v2_rot_cam.z)) * length };
			SDL_FPoint v3_abs = { ((v3_rot_cam.x) / (v3_rot_cam.z)) * length ,
								  ((v3_rot_cam.y) / (v3_rot_cam.z)) * length };

			// Place object in center of screen (50%, 50%)
			SDL_FPoint v1_2D = { v1_abs.x + this->center.x ,
								 v1_abs.y + this->center.y };
			SDL_FPoint v2_2D = { v2_abs.x + this->center.x ,
								 v2_abs.y + this->center.y };
			SDL_FPoint v3_2D = { v3_abs.x + this->center.x ,
								 v3_abs.y + this->center.y };

			// Create white color
			//SDL_Color face_color = color_factor(obj.color, sin(normalise_normal * (M_PI / 2)));
			SDL_FColor face_color = color_factor(face.mtl->Kd, std::max(normalise_normal, this->global_light));

			// Create vertex for geometry
			SDL_Vertex vx1 = { v1_2D, face_color, face.vt1 };
			SDL_Vertex vx2 = { v2_2D, face_color, face.vt2 };
			SDL_Vertex vx3 = { v3_2D, face_color, face.vt3 };

			// Create list of vertex to display
			if (face.mtl == nullptr) {
				SDL_Log("Error material empty");
			}
			vertex_geometry vxs = { vx1, vx2, vx3 , (v1_pos.z + v2_pos.z + v3_pos.z) / 3.f, face.mtl };
			trigon.push_back(vxs);

		}
	}
	std::sort(trigon.begin(), trigon.end(), [](vertex_geometry& f1, vertex_geometry& f2) {
		return f1.z > f2.z;
		});
	
	for (vertex_geometry face : trigon) {
		SDL_Vertex vxs[] = { face.v1, face.v2, face.v3 };
		SDL_SetRenderDrawColor(this->renderer, face.mtl->Kd.r*255, face.mtl->Kd.g*255, face.mtl->Kd.b*255, face.mtl->d*255);
		SDL_RenderGeometry(this->renderer, face.mtl->map_Kd, vxs, 3, nullptr, 0);

		// Rendering mesh
		if (this->mesh_display) {
			SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderLine(this->renderer, face.v1.position.x, face.v1.position.y, face.v2.position.x, face.v2.position.y);
			SDL_RenderLine(this->renderer, face.v2.position.x, face.v2.position.y, face.v3.position.x, face.v3.position.y);
			SDL_RenderLine(this->renderer, face.v3.position.x, face.v3.position.y, face.v1.position.x, face.v1.position.y);
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

SDL_FColor color_factor(SDL_FColor color, float factor)
{
	return { (color.r * factor), (color.g * factor), (color.b * factor), (float)color.a };
}

vertice_3_f centroid(vertice_3_f v1, vertice_3_f v2, vertice_3_f v3) {
	return {(v1.x+v2.x+v3.x)/3.f,(v1.y+v2.y+v3.y)/3.f,(v1.z+v2.z+v3.z)/3.f};
}