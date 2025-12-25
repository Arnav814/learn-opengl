#ifndef SDLCONFIG_HPP
#define SDLCONFIG_HPP
#include "common.hpp"
#include "shaders.hpp"

#include <glm/ext/vector_uint2.hpp>

#include <SDL3/SDL.h>

#include <format>
#include <print>
#include <string>

#define CALL_SDL(...) \
	do { \
		auto retval = __VA_ARGS__; \
		if (not retval) { \
			std::string errContent = SDL_GetError(); \
			std::string errString = std::format("{} @ {}:{} returned {}: {}", #__VA_ARGS__, \
			                                    __FILE__, __LINE__, retval, errContent); \
			throw std::runtime_error(errString); \
		} \
	} while (false)

// also handles GLAD stuff
class SDLData {
  public:
	SDL_Window* window;
	SDL_GLContext context;
	int version;
	glm::uvec2 initSize; // initial size

	// init this instance
	void setup(const glm::uvec2 initSize);

	// returns a value in pixels
	glm::ivec2 getWindowSize();

	// clean things up. should be called once at the end of the program
	void destroy();
};

#endif /* SDLCONFIG_HPP */
