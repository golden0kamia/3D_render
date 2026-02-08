/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/


#include <main.hpp>

#define WINDOW_WIDTH (1920)
#define WINDOW_HEIGHT (1080)

using namespace std;

int main(int argc, char* argv[]) {
	/*fastgltf::Parser parser;
	std::filesystem::path path = R"(C:\Users\Bastien\Desktop\cube.glb)";
	auto gltf_file = fastgltf::GltfDataBuffer::FromPath(path);
	auto asset = parser.loadGltf(gltf_file.get(), path.parent_path(), fastgltf::Options::None);*/
	SDL_Log("%s", "Start 3D Render\n");

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
		return -10;
	}

	// Create window
	SDL_Window* gWindow = SDL_CreateWindow("3D Render", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (gWindow == nullptr) {
		SDL_LogError(0, "%s", SDL_GetError());
		return -2;
	}
	// Create renderer
	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, nullptr);
	if (gRenderer == nullptr) {
		SDL_LogError(0, "%s", SDL_GetError());
		return -3;
	}

	//Clear Render with black color
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gRenderer);

	SDL_RenderPresent(gRenderer);
	camera camera_1(gRenderer);
	{
		camera_1.set_screen_size((SDL_Point){ WINDOW_WIDTH, WINDOW_HEIGHT });
		camera_1.fov = 90;
		camera_1.position = (vertice_3_f){ 0., 0., -20. };
		//camera_1.angle = { 0., -M_PI/2., 0. };
	}

	// Load file object to memory
	std::vector<object> objects;
	SDL_ShowOpenFileDialog(file_callback, &objects, nullptr, nullptr, 0, nullptr, false);
	load_object(R"(C:\Users\Bastien\Projets\3D_render\objects\test_cube.obj)", &objects, gRenderer);

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to the latest version of SDL is recommended!)
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		printf("Error: %s\n", SDL_GetError());
		return -5;
	}

#ifdef DEBUG_WINDOW
	// Setup window
	SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", 640, 480, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -6;
	}

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100 (WebGL 1.0)
		const char* glsl_version = "#version 100";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
		// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
		const char* glsl_version = "#version 300 es";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
		// GL 3.2 Core + GLSL 150
		const char* glsl_version = "#version 150";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		SDL_GLContext gl_context = SDL_GL_CreateContext(window);
		if (gl_context == nullptr)
		{
			printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
			return 1;
		}

		SDL_GL_MakeCurrent(window, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		SDL_ShowWindow(window);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

		// Setup scaling
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init(glsl_version);

#endif

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	SDL_Event Event;
	bool run = true;
	bool vsync = true;

	unsigned int fps;
	unsigned long long ticks = SDL_GetTicks();

	while (run) {

#ifdef DEBUG_WINDOW
		// Start the Dear ImGui frame
		{
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		}

		//ImGui::ShowDemoWindow(&show_demo_window);
		
		// Display settings
		{
			ImGui::Begin("Move object", &run);
			unsigned int  diff_ticks = SDL_GetTicks() - ticks;
			ticks = SDL_GetTicks();
			fps = static_cast<unsigned int>(1000. / average_ticks(diff_ticks));
			ImGui::Value("FPS", fps);
			ImGui::Value("Ticks", (unsigned int)diff_ticks);
			ImGui::Checkbox("vsync", &vsync);
			ImGui::SliderInt("FOV", &camera_1.fov, 1, 179);
			ImGui::Value("Length", camera_1.length());
			ImGui::Checkbox("Mesh", &camera_1.mesh_display);
			ImGui::SliderFloat("Global light", &camera_1.global_light, 0.f, 1.f);
			ImGui::Separator();
			ImGui::SliderFloat("Camera x", &camera_1.position.x, -WINDOW_WIDTH / 2.f, WINDOW_WIDTH / 2.f);
			ImGui::SliderFloat("Camera y", &camera_1.position.y, -WINDOW_HEIGHT / 2.f, WINDOW_HEIGHT / 2.f);
			ImGui::SliderFloat("Camera z", &camera_1.position.z, -100., 100.);
			ImGui::SliderFloat("Camera sx", &camera_1.size.x, 0., 100.);
			ImGui::SliderFloat("Camera sy", &camera_1.size.y, 0., 100.);
			ImGui::SliderFloat("Camera sz", &camera_1.size.z, 0., 100.);
			ImGui::SliderFloat("Camera rx", &camera_1.angle.x, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Camera ry", &camera_1.angle.y, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Camera rz", &camera_1.angle.z, (float)-M_PI, (float)M_PI);
			ImGui::Value("Camera normal x", camera_1.normal.x);
			ImGui::Value("Camera normal y", camera_1.normal.y);
			ImGui::Value("Camera normal z", camera_1.normal.z);
			for (object &obj : objects) {
				float rotate = obj.angle.z + 0.001f* diff_ticks;
				//obj.angle.z = rotate > 2 * M_PI ? 0 : rotate;
				ImGui::Checkbox(obj.name.c_str(), &obj.display);
			}
			ImGui::SliderFloat("Object rx", &objects[0].angle.x, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Object ry", &objects[0].angle.y, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Object rz", &objects[0].angle.z, (float)-M_PI, (float)M_PI);
			//cube.settings();
			ImGui::End();
		}


		// Rendering
		{
			ImGui::Render();
			glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(window);
		}
#endif


		// Render of 3D scene
		SDL_SetRenderDrawColor(gRenderer, camera_1.global_light * 255, camera_1.global_light * 255, camera_1.global_light * 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		camera_1.render_object(objects);

		//Event handling
		while (SDL_PollEvent(&Event)) {
			ImGui_ImplSDL3_ProcessEvent(&Event);
			switch (Event.type) {
			case SDL_EVENT_QUIT:
				run = false;
				SDL_Log("Quit window\n");
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (Event.key.key) {
				case SDLK_W:
					camera_1.position.x += 0.1f;
				default:
					break;
				}
			default:
				//std::cout << Event.type << std::endl;
				break;
			}
		}
		SDL_RenderPresent(gRenderer);
	}

#ifdef DEBUG_WINDOW
	// Cleanup IMGUI
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
		SDL_GL_DestroyContext(gl_context);
		SDL_DestroyWindow(window);
	}
#endif

	{
		SDL_DestroyRenderer(gRenderer);
		SDL_DestroyWindow(gWindow);
		SDL_Quit();
	}

	SDL_Log("End 3D Render\n");
	return 0;
}

unsigned int average_ticks(unsigned int ticks) {
	static unsigned int ticks_list[100];
	static int index = 0;
	ticks_list[index] = ticks;
	unsigned long long sum = 0;
	for (unsigned int tick : ticks_list) {
		sum += tick;
	}
	index++;
	if (index == 100) index = 0;
	return static_cast<unsigned int>(sum / 100);
}

void file_callback(void *userdata, const char * const *filelist, int filter) {
	if (filelist == nullptr) {
		SDL_Log("Error in file dialog\n");
		return;
	}
	if (*filelist == nullptr) {
		SDL_Log("Canceled or no file selected\n");
		return;
	}
	while (*filelist != nullptr) {
		cout << "file path: " << *filelist << endl;
		filelist++;
	}
}

int load_object(std::filesystem::path path, std::vector<object> *objects, SDL_Renderer* renderer)
{
	// Create temporary new material list
	std::vector<material*> materials;
	//std::filesystem::path path(R"(C:\Users\Bastien\Projets\3D_render\objects\TinyMage.obj)");
	std::ifstream file(path);
	path.remove_filename();
	// Create temporary new vertice list
	std::vector<vertice_3_f> vertices;
	// Create temporary new point list
	std::vector<SDL_FPoint> vertices_texture;
	// Create temporary new normal vertice list
	std::vector<vertice_3_f> vertices_normal;
	material* use_mtl = nullptr;
	if (!file.is_open()) return -7;

	char line[1024];
	while (!file.eof()) {
		file.getline(line, 1024);
		std::strstream line_str;
		line_str << line;

		string type;
		line_str >> type;
		if (type == "o") {
			objects->emplace_back();
			line_str >> objects->back().name;
			SDL_Log("New object: %s", objects->back().name.c_str());
		}
		else if (type == "v") {
			vertice_3_f vtmp;
			line_str >> vtmp.x >> vtmp.y >> vtmp.z;
			vertices.push_back(vtmp);
		}
		else if (type == "vt") {
			SDL_FPoint vttmp;
			line_str >> vttmp.x >> vttmp.y;
			vertices_texture.push_back(vttmp);
		}
		else if (type == "vn") {
			vertice_3_f vntmp;
			line_str >> vntmp.x >> vntmp.y >> vntmp.z;
			vertices_normal.push_back(vntmp);
		}
		else if (type == "f") {
			int tmp;
			std::vector<int> vertices_n;
			std::vector<int> textures_n;
			std::vector<int> normals_n;
			char sepa;
			while (!line_str.eof()) {
				line_str >> tmp;
				vertices_n.push_back(tmp);
				line_str.get(sepa);
				if (sepa == '/' && !vertices_texture.empty()) {
					line_str >> tmp;
					textures_n.push_back(tmp);
					line_str.get(sepa);
				}
				if (sepa == '/' && !vertices_normal.empty()) {
					line_str >> tmp;
					normals_n.push_back(tmp);
					line_str.get(sepa);
				}
				else if (sepa == ' ') {
				}
			}
			objects->back().faces.emplace_back();
			objects->back().faces.back().vx = vertices[vertices_n[0] - 1];
			objects->back().faces.back().vy = vertices[vertices_n[1] - 1];
			objects->back().faces.back().vz = vertices[vertices_n[2] - 1];
			if (!vertices_texture.empty())
			{
				objects->back().faces.back().vt1 = vertices_texture[textures_n[0] - 1];
				objects->back().faces.back().vt2 = vertices_texture[textures_n[1] - 1];
				objects->back().faces.back().vt3 = vertices_texture[textures_n[2] - 1];
			}
			if (!vertices_normal.empty())
			{
				objects->back().faces.back().vnx = vertices_normal[normals_n[0] - 1];
				objects->back().faces.back().vny = vertices_normal[normals_n[1] - 1];
				objects->back().faces.back().vnz = vertices_normal[normals_n[2] - 1];
			}
			objects->back().faces.back().mtl = use_mtl;
		}
		else if (type == "usemtl") {
			string mtl_name;
			line_str >> mtl_name;
			SDL_Log("Use material: %s", mtl_name.c_str());
			use_mtl = nullptr;
			for (material* mtl : materials) {
				if (mtl->name == mtl_name) {
					use_mtl = mtl;
				}
			}
			if (use_mtl == nullptr) {
				SDL_Log("Error material not found");
			}
		}
		else if (type == "mtllib") {
			std::string mtl_path;
			line_str >> mtl_path;
			SDL_Log("Open Material library: %s", mtl_path.c_str());
			std::ifstream mtl(path.generic_string() + mtl_path);
			char line_m[1024];

			while (!mtl.eof()) {
				mtl.getline(line_m, 1024);
				std::strstream line_m_str;
				line_m_str << line_m;

				string type_m;
				line_m_str >> type_m;
				if (type_m == "newmtl") {
					materials.emplace_back();
					materials.back() = new material;
					line_m_str >> materials.back()->name;
					SDL_Log("New material: %s", materials.back()->name.c_str());
				}
				else if (type_m == "Ns") {
					line_m_str >> materials.back()->Ns;
				}
				else if (type_m == "Ka") {
					float r, g, b;
					line_m_str >> r >> g >> b;
					materials.back()->Ka.r = r;
					materials.back()->Ka.g = g;
					materials.back()->Ka.b = b;
				}
				else if (type_m == "Kd") {
					float r, g, b;
					line_m_str >> r >> g >> b;
					materials.back()->Kd.r = r;
					materials.back()->Kd.g = g;
					materials.back()->Kd.b = b;
				}
				else if (type_m == "Ks") {
					float r, g, b;
					line_m_str >> r >> g >> b;
					materials.back()->Ks.r = r;
					materials.back()->Ks.g = g;
					materials.back()->Ks.b = b;
				}
				else if (type_m == "Ke") {
					float r, g, b;
					line_m_str >> r >> g >> b;
					materials.back()->Ke.r = r;
					materials.back()->Ke.g = g;
					materials.back()->Ke.b = b;
				}
				else if (type_m == "Ni") {
					line_m_str >> materials.back()->Ni;
				}
				else if (type_m == "d") {
					line_m_str >> materials.back()->d;
					materials.back()->Tr = 1 / materials.back()->d;
					materials.back()->Kd.a = materials.back()->d;
				}
				else if (type_m == "Tr") {
					line_m_str >> materials.back()->Tr;
					materials.back()->d = 1 / materials.back()->Tr;
					materials.back()->Kd.a = materials.back()->d;
				}
				else if (type_m == "map_Ka") {
					string map_filename;
					line_m_str >> map_filename;
					map_filename = path.generic_string() + map_filename;
					SDL_Surface* image = IMG_Load(map_filename.c_str());
					if (image == nullptr) {
						SDL_Log("Error loading texture Ka: %s", map_filename.c_str());
					}
					materials.back()->map_Ka = SDL_CreateTextureFromSurface(renderer, image);
				}
				else if (type_m == "map_Kd") {
					string map_filename;
					line_m_str >> map_filename;
					map_filename = path.generic_string() + map_filename;
					SDL_Surface* image = IMG_Load(map_filename.c_str());
					if (image== nullptr) {
						SDL_Log("Error loading texture Kd: %s", map_filename.c_str());
					}
					materials.back()->map_Kd = SDL_CreateTextureFromSurface(renderer, image);
				}
				else if (type_m == "map_Ks") {
					string map_filename;
					line_m_str >> map_filename;
					map_filename = path.generic_string() + map_filename;
					SDL_Surface* image = IMG_Load(map_filename.c_str());
					if (image== nullptr) {
						SDL_Log("Error loading texture Ks: %s", map_filename.c_str());
					}
					materials.back()->map_Ks = SDL_CreateTextureFromSurface(renderer, image);
				}
				else if (type_m == "map_Ns") {
					string map_filename;
					line_m_str >> map_filename;
					if(map_filename[1] != ':') map_filename = path.generic_string() + map_filename;
					SDL_Surface* image = IMG_Load(map_filename.c_str());
					if (image== nullptr) {
						SDL_Log("Error loading texture Ns: %s", map_filename.c_str());
					}
					materials.back()->map_Ns = SDL_CreateTextureFromSurface(renderer, image);
				}
			}
		}
	}
	return 0;
}
