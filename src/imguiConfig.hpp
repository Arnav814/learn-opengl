#ifndef IMGUICONFIG_HPP
#define IMGUICONFIG_HPP
#include <SDL3/SDL_video.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

// setup ImGui
void makeContext(SDL_GLContext glContext, SDL_Window* window);

void deleteContext();

#endif /* IMGUICONFIG_HPP */
