#include "common.hpp"
#include "loadTexture.hpp"
#include "shaders.hpp"

#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <cmath>
#include <print>
#include <string>
#include <unordered_map>

const GLuint INIT_WIDTH = 800, INIT_HEIGHT = 600;

int main(void) {
	// code without checking for errors
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window =
	    SDL_CreateWindow("[glad] GL with SDL", INIT_WIDTH, INIT_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_SetWindowResizable(window, true);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::println("GLAD version: {}", version);

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);

	// SHADERS

	const char vertexShaderSrc[] = {
#embed "./shaders/vertexShader.vert.glsl"
	    , 0};
	const char fragmentShaderSrc[] = {
#embed "./shaders/fragmentShader.frag.glsl"
	    , 0};

	ShaderProgram shaderProgram{std::string(vertexShaderSrc), std::string(fragmentShaderSrc)};

	// VERTEXES AND TRIANGLES

	constexpr float vertices[] = {
	    // position       || color          || texture coords |
	    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
	    0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
	    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
	    -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top left
	};

	constexpr int indicies[] = {
	    0, 1, 3, //
	    1, 2, 3, //
	};

	uint vertBufObj;
	uint vertAttribObj;
	uint elemBufObj;
	glGenVertexArrays(1, &vertAttribObj);
	glGenBuffers(1, &vertBufObj);
	glGenBuffers(1, &elemBufObj);
	glBindVertexArray(vertAttribObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBufObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// TEXTURES

	glActiveTexture(GL_TEXTURE0);
	uint containterTexture = loadTexture("../media/container.jpg");
	glBindTexture(GL_TEXTURE_2D, containterTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	uint faceTexture = loadTexture("../media/awesomeface.png");
	glBindTexture(GL_TEXTURE_2D, faceTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	shaderProgram.use();
	shaderProgram.setUniform("containterTexture", 0);
	shaderProgram.setUniform("faceTexture", 1);

	// RENDER LOOP

	bool exit = false;
	bool resized = false;
	signed char arrowMovement = 0; // -1, 0, or 1
	float opacityVal = 0.0;
	while (not exit) {
		arrowMovement = 0;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT: exit = 1; break;
			case SDL_EVENT_KEY_UP:
				if (event.key.key == SDLK_ESCAPE) {
					exit = true;
				} else if (event.key.scancode == SDL_SCANCODE_UP) {
					arrowMovement += 1;
				} else if (event.key.scancode == SDL_SCANCODE_DOWN) {
					arrowMovement -= 1;
				}
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: resized = true; break;
			default: break;
			}
		}

		if (resized) {
			int width, height;
			SDL_GetWindowSizeInPixels(window, &width, &height);
			glViewport(0, 0, width, height);
		}

		float secsSinceInit [[maybe_unused]] = static_cast<float>(SDL_GetTicks()) / 1000.f;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		opacityVal += ((float)arrowMovement) / 10.0f;
		opacityVal = std::max(0.0f, std::min(1.0f, opacityVal));
		shaderProgram.setUniform("opacity", opacityVal);
		glBindVertexArray(vertAttribObj);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(window);
		SDL_Delay(1'000 / 60);
	}

	glDeleteVertexArrays(1, &vertAttribObj);
	glDeleteBuffers(1, &vertBufObj);
	glDeleteBuffers(1, &elemBufObj);
	shaderProgram.~ShaderProgram();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
