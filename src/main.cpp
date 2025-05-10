#include "common.hpp"
#include "loadTexture.hpp"
#include "shaders.hpp"

#include <glad/gl.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec4.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <cmath>
#include <print>
#include <string>

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

	// PERSPECTIVE

	// updated in render loop
	glm::mat4 obj2world = glm::mat4(1);
	glm::mat4 world2cam = glm::mat4(1);
	glm::mat4 projection = glm::mat4(1);
	
	glEnable(GL_DEPTH_TEST);

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
	    // clang-format off
	    // position       || texture coords |
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0
	    // clang-format on
	};

	// TODO: proper indexed drawing
	constexpr int indicies[] = {
	    0,  1,  2, //
	    3,  4,  5, //
	    6,  7,  8, //
	    9,  10, 11, //
	    12, 13, 14, //
	    15, 16, 17, //
	    18, 19, 20, //
	    21, 22, 23, //
	    24, 25, 26, //
	    27, 28, 29, //
	    30, 31, 32, //
	    33, 34, 35 //
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
	bool resized = true; // populate the perspective matrix
	while (not exit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT: exit = 1; break;
			case SDL_EVENT_KEY_UP:
				if (event.key.key == SDLK_ESCAPE) {
					exit = true;
				}
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: resized = true; break;
			default: break;
			}
		}

		if (resized) {
			int width, height;
			SDL_GetWindowSizeInPixels(window, &width, &height);
			projection = glm::perspective(45.f, (float)width / height, 0.1f, 100.f);
			glViewport(0, 0, width, height);
		}

		float secsSinceInit = static_cast<float>(SDL_GetTicks()) / 1000.f;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		obj2world =
		    glm::rotate(glm::mat4(1.f), secsSinceInit * glm::radians(-55.f), glm::vec3(1, 0.5, 0));
		obj2world = glm::scale(obj2world, glm::vec3(1.5, 1.5, 1.5)); // It looks small to me
		world2cam =
		    glm::translate(glm::mat4(1.f), -glm::vec3(0, 0, 3)); // translate in reverse direction
		shaderProgram.setUniform("obj2world", obj2world);
		shaderProgram.setUniform("world2cam", world2cam);
		shaderProgram.setUniform("projection", projection);

		glBindVertexArray(vertAttribObj);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
