#include "camera.hpp"
#include "common.hpp"
#include "genTerrain.hpp"
#include "lighting.hpp"
#include "model.hpp"
#include "shaders.hpp"
#include "vertexData.hpp"

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

constexpr uint INIT_WIDTH = 800, INIT_HEIGHT = 600;

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

int main(void) {
	CALL_SDL(SDL_Init(SDL_INIT_VIDEO));

	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
	CALL_SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4));

	SDL_Window* window =
	    SDL_CreateWindow("[glad] GL with SDL", INIT_WIDTH, INIT_HEIGHT, SDL_WINDOW_OPENGL);
	CALL_SDL(SDL_SetWindowResizable(window, true));
	CALL_SDL(SDL_SetWindowRelativeMouseMode(window, true));

	SDL_GLContext context = SDL_GL_CreateContext(window);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::println("GLAD version: {}", version);

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);

	// PERSPECTIVE

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Camera camera{glm::vec2(INIT_WIDTH, INIT_HEIGHT)};
	camera.setPosition(glm::vec3(0, 0, 3));

	LightComponents baseLightColor{
	    .ambient = glm::vec3(.1),
	    .diffuse = glm::vec3(.5),
	    .specular = glm::vec3(.5),
	};

	AttenuationComponents baseAttenuation{
	    .constant = 1.0,
	    .linear = 0.09,
	    .quadratic = 0.032,
	};

	const DirLight dirLight{
	    .direction = glm::normalize(glm::vec3(0.1, -1, 0.1)),
	    .components = baseLightColor,
	};

	std::vector<PointLight> pointLights = getPointLights();

	glm::vec3 spotLightColor = glm::vec3(1, 1, 0.8); // yellowish
	// translate to the appropriate position before use
	const SpotLight baseSpotLight{
	    .position = origin3d, // placeholder
	    .direction = origin3d, // placeholder
	    .components = baseLightColor * spotLightColor,
	    .attenuation = baseAttenuation,
	    .inCutoff = glm::cos(glm::radians(12.5f)),
	    .outCutoff = glm::cos(glm::radians(17.5f)),

	};

	// SHADERS

	std::print("Compiling shaders... ");
	std::fflush(stdout);
	std::shared_ptr objShader = ShaderProgram::make(SOURCE_DIR "./shaders/object.vert.glsl",
	                                                SOURCE_DIR "./shaders/object.frag.glsl");
	std::shared_ptr terrainShader = ShaderProgram::make(SOURCE_DIR "./shaders/terrain.vert.glsl",
	                                                    SOURCE_DIR "./shaders/terrain.frag.glsl");
	std::shared_ptr lightShader = ShaderProgram::make(SOURCE_DIR "./shaders/lightCube.vert.glsl",
	                                                  SOURCE_DIR "./shaders/lightCube.frag.glsl");
	std::println("Done.");

	// MODELS

	std::print("Loading models... ");
	std::fflush(stdout);

	std::shared_ptr<SceneGraphRoot> scene = std::make_shared<SceneGraphRoot>();
	scene->addChild(
	    std::shared_ptr<Model>(new Model{MEDIA_DIR "./backpack/backpack.obj", objShader}));

	std::println("Done.");

	// TERRAIN

	std::print("Generating terrain... ");
	std::fflush(stdout);
	Terrain terrain{123'123,
	                   32,
	                   glm::vec3(0.96, 0.84, 0.69),
	                   glm::vec3(0.25, 0.60, 0.04),
	                   glm::vec2(50),
	                   glm::ivec2(250),
	                   terrainShader};
	scene->addChild(std::make_shared<Terrain>(terrain));
	std::println("Done.");

	std::vector<SceneCascade> stack{};
	recursivelyPrint(scene, stack);

	// LIGHT BUFFERS

	uint lightVBO;
	glGenBuffers(1, &lightVBO);
	uint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	// populate
	std::vector<float> verticies = getVertexData();
	glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE_BYTES(verticies), verticies.data(), GL_STATIC_DRAW);

	// only position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// RENDER LOOP

	// map for if each scancode is pressed
	std::array<bool, SDL_SCANCODE_COUNT> scancodeMap{false};

	bool isFullscreen = false;

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
				case SDLK_T:
					isFullscreen = not isFullscreen; // toggle fullscreen
					SDL_SetWindowFullscreen(window, isFullscreen);
					break;
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
			CALL_SDL(SDL_GetWindowSizeInPixels(window, &width, &height));
			camera.setWindowSize(glm::ivec2(width, height));
			glViewport(0, 0, width, height);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(lightVAO);

		objShader->use();

		dirLight.setStructUniform(*objShader, "dirLight");

		for (uint i = 0; i < pointLights.size(); i++) {
			pointLights[i].setStructUniform(*objShader, "pointLights", i);
		}

		// move spotlight to camera to act as a flashlight
		SpotLight flashlight = baseSpotLight;
		flashlight.position = camera.getPosition();
		flashlight.direction = camera.getFront();
		flashlight.setStructUniform(*objShader, "spotLight");

		terrainShader->use();

		dirLight.setStructUniform(*terrainShader, "dirLight");

		for (uint i = 0; i < pointLights.size(); i++) {
			pointLights[i].setStructUniform(*terrainShader, "pointLights", i);
		}

		// move spotlight to camera to act as a flashlight
		flashlight = baseSpotLight;
		flashlight.position = camera.getPosition();
		flashlight.direction = camera.getFront();
		flashlight.setStructUniform(*terrainShader, "spotLight");

		std::vector<SceneCascade> stack{};
		recursivelyRender(scene, camera, stack);

		glBindVertexArray(0);
		objShader->stopUsing();

		for (uint i = 0; i < pointLights.size(); i++) {
			pointLights[i].vizualize(*lightShader, camera, lightVAO);
		}
		dirLight.vizualize(*lightShader, camera, lightVAO);

		lastFrameTime = secsSinceInit;
		SDL_GL_SwapWindow(window);
		SDL_Delay(1'000 / 60);
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);

	CALL_SDL(SDL_GL_DestroyContext(context));
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
