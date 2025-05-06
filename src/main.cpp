#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace chrono = std::chrono;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;
// The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;
// The image we will load and show on the screen
SDL_Surface* gHelloWorld = nullptr;

// Initialize SDL
void init() {
	bool success = true;
	if (not SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Create window
		if (gWindow = SDL_CreateWindow("SDL3 Tutorial: Hello SDL3", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		    gWindow == nullptr) {
			SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
			success = false;
		} else {
			// Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	if (not success) throw std::runtime_error("Failed to initialize SDL3!");
}

void close() {
	// Clean up surface
	SDL_DestroySurface(gHelloWorld);
	gHelloWorld = nullptr;

	// Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gScreenSurface = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

int main() {
	init();
	std::this_thread::sleep_for(chrono::seconds(10));
	close();

	return 0;
}
