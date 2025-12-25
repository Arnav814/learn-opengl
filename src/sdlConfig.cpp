#include "sdlConfig.hpp"

void SDLData::setup(const glm::uvec2 initSize) {
	this->initSize = initSize;
	CALL_SDL(SDL_Init(SDL_INIT_VIDEO));

	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4));

	this->window = SDL_CreateWindow("[glad] GL with SDL", this->initSize.x, this->initSize.y,
	                                SDL_WINDOW_OPENGL);
	CALL_SDL(SDL_SetWindowResizable(window, true));
	// CALL_SDL(SDL_SetWindowRelativeMouseMode(window, true));

	this->context = SDL_GL_CreateContext(window);

	this->version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::println("GLAD version: {}", this->version);

	glViewport(0, 0, this->initSize.x, this->initSize.y);
}

glm::ivec2 SDLData::getWindowSize() {
	int width, height;
	CALL_SDL(SDL_GetWindowSizeInPixels(this->window, &width, &height));
	return glm::ivec2(width, height);
}

void SDLData::destroy() {
	CALL_SDL(SDL_GL_DestroyContext(context));
	SDL_DestroyWindow(window);
	SDL_Quit();
}
