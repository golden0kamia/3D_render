#include "object.hpp"

object::object()
{
}

object::object(std::string filepath)
{
	this->load(filepath);
}

int object::load(std::string filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) return -1;

	char line[128];
	while (!file.eof()) {
		file.getline(line, 128);
		std::strstream line_str;
		line_str << line;

		char type;
		switch (line[0]) {
		case 'o':
			line_str >> type >> this->name;
			break;
		case 'v':
			vertice_3_f vtmp;
			line_str >> type >> vtmp.x >> vtmp.y >> vtmp.z;
			this->vertices.push_back(vtmp);
			break;
		case 'f':
			int v1, v2, v3;
			line_str >> type >> v1 >> v2 >> v3;
			face_f ftmp = { this->vertices[v1-1], this->vertices[v2-1], this->vertices[v3-1]};
			this->faces.push_back(ftmp);
			break;
		}
	}

	return 0;
}
