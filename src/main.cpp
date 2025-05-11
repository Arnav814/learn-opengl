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
	glm::mat4 world2cam = glm::mat4(1);
	glm::mat4 projection = glm::mat4(1);

	glEnable(GL_DEPTH_TEST);

	// CAMERA

	EulerAngle playerAngle{
	    .yaw = 0.f,
	    .pitch = 0.f,
	};
	constexpr float rotateSpeed = glm::radians(0.1f); // in radians per pixel moved

	Camera camera{
	    .position = glm::vec3(0, 0, 3),
	    .front = glm::vec3(0, 0, -1),
	    .up = glm::vec3(0, 1, 0),
	};
	constexpr float cameraSpeed = 2.5; // movement per second

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
	    // position        || texture coords |
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0
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

	std::array cubePositions{
	    glm::vec3(0.0f, 0.0f, 0.0f), //
	    glm::vec3(2.0f, 5.0f, -15.0f), //
	    glm::vec3(-1.5f, -2.2f, -2.5f), //
	    glm::vec3(-3.8f, -2.0f, -12.3f), //
	    glm::vec3(2.4f, -0.4f, -3.5f), //
	    glm::vec3(-1.7f, 3.0f, -7.5f), //
	    glm::vec3(1.3f, -2.0f, -2.5f), //
	    glm::vec3(1.5f, 2.0f, -2.5f), //
	    glm::vec3(1.5f, 0.2f, -1.5f), //
	    glm::vec3(-1.3f, 1.0f, -1.5f) //
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

	// map for if each scancode is pressed
	std::array<bool, SDL_SCANCODE_COUNT> scancodeMap{false};

	bool exit = false;
	bool resized = true; // populate the perspective matrix
	float lastFrameTime = 0; // measured since init
	while (not exit) {

		float secsSinceInit = static_cast<float>(SDL_GetTicks()) / 1000.f;
		float deltaTime = secsSinceInit - lastFrameTime;
		float frameCameraSpeed = deltaTime * cameraSpeed; // normalized by time

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
			case SDL_EVENT_MOUSE_MOTION:
				playerAngle.yaw += (float)event.motion.xrel * rotateSpeed;
				playerAngle.pitch -= (float)event.motion.yrel * rotateSpeed;

				if (playerAngle.pitch > glm::radians(89.f)) playerAngle.pitch = glm::radians(89.f);
				if (playerAngle.pitch < glm::radians(-89.f))
					playerAngle.pitch = glm::radians(-89.f);

				camera.front = playerAngle.toDirection();
				break;
			}
		}

		if (scancodeMap[SDL_SCANCODE_W]) camera.position += frameCameraSpeed * camera.front;
		if (scancodeMap[SDL_SCANCODE_S]) camera.position -= frameCameraSpeed * camera.front;
		if (scancodeMap[SDL_SCANCODE_A])
			camera.position -=
			    glm::normalize(glm::cross(camera.front, camera.up)) * frameCameraSpeed;
		if (scancodeMap[SDL_SCANCODE_D])
			camera.position +=
			    glm::normalize(glm::cross(camera.front, camera.up)) * frameCameraSpeed;

		if (resized) {
			int width, height;
			SDL_GetWindowSizeInPixels(window, &width, &height);
			projection = glm::perspective(glm::radians(45.f), (float)width / height, 0.1f, 100.f);
			glViewport(0, 0, width, height);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world2cam = camera.toCamSpace();
		shaderProgram.setUniform("world2cam", world2cam);
		shaderProgram.setUniform("projection", projection);

		glBindVertexArray(vertAttribObj);
		for (uint i = 0; i < cubePositions.size(); i++) {
			glm::vec3 cubePosition = cubePositions[i];
			obj2world = glm::mat4(1);
			obj2world = glm::translate(obj2world, cubePosition);
			float angle = glm::radians(20.f * i); // random angle per cube
			obj2world = glm::rotate(obj2world, angle, glm::vec3(1, 0.5, 0));
			shaderProgram.setUniform("obj2world", obj2world);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);

		lastFrameTime = secsSinceInit;
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
