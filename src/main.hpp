/*****************************************************************//**
 * \file   main.hpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/

#include <iostream>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
//#include <SDL_syswm.h>
#include "object.hpp"
#include "camera.hpp"
#include <imgui.h>
//#include "Library/ImGui/imgui_impl_dx11.h"
//#include "Library/ImGui/imgui_impl_sdl2.h"
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl3.h>
#include <d3d11.h>

 // Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
unsigned int average_ticks(unsigned int ticks);
