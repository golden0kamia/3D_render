/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/

#include "main.hpp"

#define WINDOW_WIDTH 1920-50
#define WINDOW_HEIGHT 1080-50

using namespace std;

int main(int argc, char* argv[]) {
	SDL_Log("Start 3D Render\n");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_LogError(0, SDL_GetError());
		return -1;
	}

	// Create window
	SDL_Window* gWindow = SDL_CreateWindow("3D Render", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (gWindow == nullptr) {
		SDL_LogError(0, SDL_GetError());
		return -1;
	}
	// Create renderer
	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, NULL);
	if (gRenderer == nullptr) {
		SDL_LogError(0, SDL_GetError());
		return -1;
	}

	//Clear Render with black color
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	SDL_RenderPresent(gRenderer);
	camera camera_1(gRenderer);
	{
		camera_1.set_screen_size({ WINDOW_WIDTH, WINDOW_HEIGHT });
		camera_1.fov = 90;
		camera_1.position = { 0., 0., -20. };
		//camera_1.angle = { 0., -M_PI/2., 0. };
	}

	std::vector<object> objects;
	std::vector<material> materials;
	string filename = argv[1];
	{
		std::vector<vertice_3_f> vertices;
		std::vector<SDL_FPoint> vertices_texture;
		std::vector<vertice_3_f> vertices_normal;
		material* use_mtl = nullptr;
		std::string path_base("objects/");
		std::ifstream file(path_base + filename);
		if (!file.is_open()) return -1;

		char line[1024];
		while (!file.eof()) {
			file.getline(line, 1024);
			std::strstream line_str;
			line_str << line;

			string type;
			line_str >> type;
			if (type == "o") {
				objects.push_back(object());
				line_str >> objects.back().name;
				//SDL_Log("New object: %s", objects.back().name.c_str());
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
						continue;
					}
				}
				objects.back().faces.push_back(face_f());
				objects.back().faces.back().v1 = vertices[vertices_n[0] - 1];
				objects.back().faces.back().v2 = vertices[vertices_n[1] - 1];
				objects.back().faces.back().v3 = vertices[vertices_n[2] - 1];
				if (!vertices_texture.empty())
				{
					objects.back().faces.back().vt1 = vertices_texture[textures_n[0] - 1];
					objects.back().faces.back().vt2 = vertices_texture[textures_n[1] - 1];
					objects.back().faces.back().vt3 = vertices_texture[textures_n[2] - 1];
				}
				if (!vertices_normal.empty())
				{
					objects.back().faces.back().vn1 = vertices_normal[normals_n[0] - 1];
					objects.back().faces.back().vn2 = vertices_normal[normals_n[1] - 1];
					objects.back().faces.back().vn3 = vertices_normal[normals_n[2] - 1];
				}
				objects.back().faces.back().mtl = use_mtl;
			}
			else if (type == "usemtl") {
				string mtl_name;
				line_str >> mtl_name;
				//SDL_Log("Use material: %s", mtl_name.c_str());
				use_mtl = nullptr;
				for (material &mtl : materials) {
					if (mtl.name == mtl_name) {
						use_mtl = &mtl;
					}
				}
				if (use_mtl == nullptr) {
					SDL_Log("Error material not found");
				}
			}
			else if (type == "mtllib") {
				std::string mtl_path;
				line_str >> mtl_path;
				//SDL_Log("Open Material library: %s", mtl_path.c_str());
				std::ifstream mtl(path_base + mtl_path);
				char line_m[1024];

				while (!mtl.eof()) {
					mtl.getline(line_m, 1024);
					std::strstream line_m_str;
					line_m_str << line_m;

					string type_m;
					line_m_str >> type_m;
					if (type_m == "newmtl") {
						materials.push_back(material());
						line_m_str >> materials.back().name;
						//SDL_Log("New material: %s", materials.back().name.c_str());
					}
					else if (type_m == "Ns") {
						line_m_str >> materials.back().Ns;
					}
					else if (type_m == "Ka") {
						float r, g, b;
						line_m_str >> r >> g >> b;
						materials.back().Ka.r = r * 255;
						materials.back().Ka.g = g * 255;
						materials.back().Ka.b = b * 255;
					}
					else if (type_m == "Kd") {
						float r, g, b;
						line_m_str >> r >> g >> b;
						materials.back().Kd.r = r * 255;
						materials.back().Kd.g = g * 255;
						materials.back().Kd.b = b * 255;
					}
					else if (type_m == "Ks") {
						float r, g, b;
						line_m_str >> r >> g >> b;
						materials.back().Ks.r = r * 255;
						materials.back().Ks.g = g * 255;
						materials.back().Ks.b = b * 255;
					}
					else if (type_m == "Ke") {
						float r, g, b;
						line_m_str >> r >> g >> b;
						materials.back().Ke.r = r * 255;
						materials.back().Ke.g = g * 255;
						materials.back().Ke.b = b * 255;
					}
					else if (type_m == "Ni") {
						line_m_str >> materials.back().Ni;
					}
					else if (type_m == "d") {
						line_m_str >> materials.back().d;
						materials.back().Tr = 1 / materials.back().d;
						materials.back().Kd.a = materials.back().d*255;
					}
					else if (type_m == "Tr") {
						line_m_str >> materials.back().Tr;
						materials.back().d = 1 / materials.back().Tr;
						materials.back().Kd.a = materials.back().d*255;
					}
					else if (type_m == "map_Ka") {
						string filename;
						line_m_str >> filename;
						filename = path_base + filename;
						SDL_Surface* image = IMG_Load(filename.c_str());
						materials.back().map_Ka = SDL_CreateTextureFromSurface(gRenderer, image);
					}
					else if (type_m == "map_Kd") {
						string filename;
						line_m_str >> filename;
						filename = path_base + filename;
						SDL_Surface* image = IMG_Load(filename.c_str());
						materials.back().map_Kd = SDL_CreateTextureFromSurface(gRenderer, image);
					}
					else if (type_m == "map_Ks") {
						string filename;
						line_m_str >> filename;
						filename = path_base + filename;
						SDL_Surface* image = IMG_Load(filename.c_str());
						materials.back().map_Ks = SDL_CreateTextureFromSurface(gRenderer, image);
					}
					else if (type_m == "map_Ns") {
						string filename;
						line_m_str >> filename;
						if(filename[1] != ':') filename = path_base + filename;
						SDL_Surface* image = IMG_Load(filename.c_str());
						materials.back().map_Ns = SDL_CreateTextureFromSurface(gRenderer, image);
					}
				}
			}
		}
	}

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to the latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Setup window
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+DirectX11 example", 640, 480, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}

	{
		/*SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window, &wmInfo);
		HWND hwnd = (HWND)wmInfo.info.win.window;

		// Initialize Direct3D
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			return 1;
		}*/

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForD3D(window);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	}

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
	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	SDL_Event Event;
	bool run = true;
	bool vsync = true;

	unsigned int fps = 60;
	unsigned long long ticks = SDL_GetTicks();

	int r = 255, g = 255, b = 255;

	while (run) {

		
		// Start the Dear ImGui frame
		{
			
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		}

		//ImGui::ShowDemoWindow(&show_demo_window);
		
		// Display settings
		{
			ImGui::Begin("Move object", &run);
			unsigned int  diff_ticks = SDL_GetTicks() - ticks;
			ticks = SDL_GetTicks();
			fps = (unsigned int)(1000. / average_ticks(diff_ticks));
			ImGui::Value("FPS", fps);
			ImGui::Value("Ticks", (unsigned int)diff_ticks);
			ImGui::Checkbox("vsync", &vsync);
			ImGui::SliderInt("FOV", &camera_1.fov, 1, 179);
			ImGui::Value("Length", camera_1.length());
			ImGui::Checkbox("Mesh", &camera_1.mesh_display);
			ImGui::SliderFloat("Global light", &camera_1.global_light, 0.f, 1.f);
			ImGui::Separator();
			ImGui::SliderFloat("Camera x", &camera_1.position.x, -WINDOW_WIDTH / 2, WINDOW_WIDTH / 2);
			ImGui::SliderFloat("Camera y", &camera_1.position.y, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2);
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
				float rotate = obj.angle.z + 0.001* diff_ticks;
				//obj.angle.z = rotate > 2 * M_PI ? 0 : rotate;
				ImGui::Checkbox(obj.name.c_str(), &obj.display);
			}
			ImGui::SliderFloat("Object rx", &objects[0].angle.x, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Object ry", &objects[0].angle.y, (float)-M_PI, (float)M_PI);
			ImGui::SliderFloat("Object rz", &objects[0].angle.z, (float)-M_PI, (float)M_PI);
			//cube.settings();
			ImGui::End();
		}


		// Renderinga
		{
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			g_pSwapChain->Present(vsync, 0); // Present with vsync
			//g_pSwapChain->Present(0, 0); // Present without vsync
		}



		// Render of 3D scene
		SDL_SetRenderDrawColor(gRenderer, camera_1.global_light * 255, camera_1.global_light * 255, camera_1.global_light * 255, 255);
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
					camera_1.position.x =+ 0.1f;
				}
			default:
				//std::cout << Event.type << std::endl;
				break;
			}
		}
		SDL_RenderPresent(gRenderer);
	}

	// Cleanup
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
		CleanupDeviceD3D();
	}

	SDL_DestroyWindow(window);

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

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
	return (unsigned int)(sum / 100);
}

// Helper functions to use DirectX11
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}
