#include "camera.hpp"
#include "common.hpp"
#include "loadTexture.hpp"
#include "shaders.hpp"

#include <glad/gl.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
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
	SDL_SetWindowRelativeMouseMode(window, true);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::println("GLAD version: {}", version);

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);

	// PERSPECTIVE

	// updated in render loop
	glm::mat4 obj2world = glm::mat4(1);

	glEnable(GL_DEPTH_TEST);

	Camera camera{glm::vec2(INIT_WIDTH, INIT_HEIGHT)};
	camera.setPosition(glm::vec3(0, 0, 3));

	constexpr glm::vec3 lightPos = glm::vec3(1.2, 1, 2);

	// SHADERS

	const char vertexShaderSrc[] = {
#embed "./shaders/all.vert.glsl"
	    , 0};
	const char objFragShaderSrc[] = {
#embed "./shaders/object.frag.glsl"
	    , 0};
	const char lightFragShaderSrc[] = {
#embed "./shaders/lightCube.frag.glsl"
	    , 0};

	ShaderProgram objShader{std::string(vertexShaderSrc), std::string(objFragShaderSrc)};
	ShaderProgram lightShader{std::string(vertexShaderSrc), std::string(lightFragShaderSrc)};

	// VERTEXES AND TRIANGLES

	constexpr float vertices[] = {
	    // clang-format off
	    // position
		-0.5f, -0.5f, -0.5f,//
		 0.5f, -0.5f, -0.5f,//
		 0.5f,  0.5f, -0.5f,//
		-0.5f,  0.5f, -0.5f,//
		-0.5f, -0.5f,  0.5f,//
		 0.5f, -0.5f,  0.5f,//
		 0.5f,  0.5f,  0.5f,//
		-0.5f,  0.5f,  0.5f,//
		-0.5f,  0.5f,  0.5f,//
		-0.5f,  0.5f, -0.5f,//
		-0.5f, -0.5f, -0.5f,//
		 0.5f,  0.5f,  0.5f,//
		 0.5f, -0.5f, -0.5f,//
		 0.5f, -0.5f,  0.5f,//
		 0.5f, -0.5f, -0.5f,//
		-0.5f,  0.5f,  0.5f,//
		-0.5f,  0.5f, -0.5f,//
	    // clang-format on
	};

	constexpr int indicies[] = {
	    0,  1,  2, //
	    2,  3,  0, //
	    4,  5,  6, //
	    6,  7,  4, //
	    8,  9,  10, //
	    10, 4,  8, //
	    11, 2,  12, //
	    12, 13, 11, //
	    10, 14, 5, //
	    5,  4,  10, //
	    3,  2,  11, //
	    11, 15, 16 //
	};

	// CONTAINER BUFFERS

	uint vertBufObj;
	uint objVertAttribObj;
	uint elemBufObj;
	glGenVertexArrays(1, &objVertAttribObj);
	glGenBuffers(1, &vertBufObj);
	glGenBuffers(1, &elemBufObj);
	glBindVertexArray(objVertAttribObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBufObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// LIGHT BUFFERS

	uint lightVertAttribObj;
	glGenVertexArrays(1, &lightVertAttribObj);
	glBindVertexArray(lightVertAttribObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	objShader.use();
	objShader.setUniform("lightColor", glm::vec3(1, .5, .31));
	objShader.setUniform("objectColor", glm::vec3(1, 1, 1));
	objShader.stopUsing();

	// RENDER LOOP

	// map for if each scancode is pressed
	std::array<bool, SDL_SCANCODE_COUNT> scancodeMap{false};

	bool exit = false;
	bool resized = true; // populate the perspective matrix
	float lastFrameTime = 0; // measured since init
	while (not exit) {

		float secsSinceInit = static_cast<float>(SDL_GetTicks()) / 1000.f;
		float deltaTime = secsSinceInit - lastFrameTime;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT: exit = 1; break;
			case SDL_EVENT_KEY_DOWN: scancodeMap.at(event.key.scancode) = true; break;
			case SDL_EVENT_KEY_UP:
				switch (event.key.key) {
				case SDLK_ESCAPE: exit = true; break;
				}
				scancodeMap.at(event.key.scancode) = false;
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: resized = true; break;
			case SDL_EVENT_MOUSE_MOTION: camera.rotateBy(event.motion); break;
			case SDL_EVENT_MOUSE_WHEEL: camera.zoomBy(event.wheel); break;
			}
		}

		camera.moveBy(scancodeMap[SDL_SCANCODE_W], //
		              scancodeMap[SDL_SCANCODE_S], //
		              scancodeMap[SDL_SCANCODE_A], //
		              scancodeMap[SDL_SCANCODE_D], //
		              scancodeMap[SDL_SCANCODE_R], //
		              scancodeMap[SDL_SCANCODE_F], //
		              deltaTime);

		if (resized) {
			int width, height;
			SDL_GetWindowSizeInPixels(window, &width, &height);
			camera.setWindowSize(glm::ivec2(width, height));
			glViewport(0, 0, width, height);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objShader.use();
		glBindVertexArray(objVertAttribObj);
		obj2world = glm::mat4(1);
		objShader.setUniform("world2cam", camera.toCamSpace());
		objShader.setUniform("projection", camera.projectionMat());
		objShader.setUniform("obj2world", obj2world);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		objShader.stopUsing();

		lightShader.use();
		glBindVertexArray(lightVertAttribObj);
		obj2world = glm::mat4(1);
		obj2world = glm::translate(obj2world, lightPos);
		obj2world = glm::scale(obj2world, glm::vec3(0.2));
		lightShader.setUniform("world2cam", camera.toCamSpace());
		lightShader.setUniform("projection", camera.projectionMat());
		lightShader.setUniform("obj2world", obj2world);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		lightShader.stopUsing();

		lastFrameTime = secsSinceInit;
		SDL_GL_SwapWindow(window);
		SDL_Delay(1'000 / 60);
	}

	glDeleteVertexArrays(1, &objVertAttribObj);
	glDeleteBuffers(1, &vertBufObj);
	glDeleteBuffers(1, &elemBufObj);
	objShader.~ShaderProgram();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
