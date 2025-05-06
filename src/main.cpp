// gcc example/c/gl_sdl2.c build/src/gl.c -Ibuild/include `sdl2-config --libs --cflags` -ldl
#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

const GLuint WIDTH = 800, HEIGHT = 600;

int main(void) {
	// code without checking for errors
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("[glad] GL with SDL", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	SDL_SetWindowResizable(window, true);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	int version [[maybe_unused]] = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

	glViewport(0, 0, 800, 600);

	int exit = 0;
	while (not exit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT: exit = 1; break;
			case SDL_EVENT_KEY_UP:
				if (event.key.key == SDLK_ESCAPE) {
					exit = 1;
				}
				break;
			default: break;
			}
		}

		glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
		SDL_Delay(1);
	}

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
