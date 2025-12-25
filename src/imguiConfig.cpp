#include "imguiConfig.hpp"

void makeContext(SDL_GLContext glContext, SDL_Window* window) {
	// copied in part from
	// https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_opengl3/main.cpp
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	// Setup scaling
	float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(mainScale);
	style.FontScaleDpi = mainScale;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void deleteContext() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
