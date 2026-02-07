/*****************************************************************//**
 * \file   main.hpp
 * \brief  
 * 
 * \author golde
 * \date   April 2024
 *********************************************************************/
#ifndef MAIN_HPP
#define MAIN_HPP
#define DEBUG_WINDOW

#include <iostream>
#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <object.hpp>
#include <camera.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#ifdef DEBUG_WINDOW
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>
#include <backends/imgui_impl_sdl3.h>
#endif

unsigned int average_ticks(unsigned int ticks);
void file_callback(void *userdata, const char * const *filelist, int filter);
int load_object(std::filesystem::path path, std::vector<object> *objects, SDL_Renderer * renderer);

#endif // MAIN_HPP
