#include "camera.hpp"
#include "common.hpp"
#include "genTerrain.hpp"
#include "imguiConfig.hpp"
#include "lightCube.hpp"
#include "lighting.hpp"
#include "model.hpp"
#include "object.hpp"
#include "sceneConf.hpp"
#include "sdlConfig.hpp"
#include "shaders.hpp"
#include "terrain.hpp"
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

#include <imgui.h>

#include <cmath>
#include <print>
#include <string>

int main(int argc, char** argv) {
	std::shared_ptr<Config> conf = parseArgs(argc, argv);
	if (conf == NULL) return 0;

	// SDL

	SDLData sdl;
	sdl.setup({800, 600});

	// PERSPECTIVE

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Camera camera{glm::vec2(sdl.initSize)};
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

	const Shaders::DirectionalLight dirLight{.direction = glm::normalize(glm::vec3(0.1, -1, 0.1)),
	                                         SET_LIGHT_COMPONENTS(baseLightColor)};

	std::vector<Shaders::PointLight> pointLights = getPointLights();

	glm::vec3 spotLightColor = glm::vec3(1, 1, 0.8); // yellowish
	// translate to the appropriate position before use
	const Shaders::SpotLight baseSpotLight{
	    .position = origin3d, // placeholder
	    .direction = origin3d, // placeholder
	    SET_LIGHT_COMPONENTS(baseLightColor * spotLightColor),
	    SET_LIGHT_ATTENUATION(baseAttenuation),
	    .inCutoff = glm::cos(glm::radians(12.5f)),
	    .outCutoff = glm::cos(glm::radians(17.5f)),

	};

	// SHADERS

	std::print("Compiling shaders... ");
	std::fflush(stdout);
	ShaderContainer shaders{
	    .objShader = Shaders::ObjectImpl::make(),
	    .terrainShader = Shaders::TerrainImpl::make(),
	    .lightShader = Shaders::LightCubeImpl::make(),
	};
	std::println("Done.");

	// SCENE
	auto scene = initScene(shaders, *conf);

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

	// IMGUI
	makeImGuiContext(sdl.context, sdl.window);

	// RENDER LOOP

	// map for if each scancode is pressed
	std::array<bool, SDL_SCANCODE_COUNT> scancodeMap{false};

	bool isFullscreen = false;
	// switch from interacting with the GUI to the camera
	bool cameraInteraction = true;
	bool showWireframe = false;
	bool displayNormals = false;

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
				scancodeMap.at(event.key.scancode) = false;
				switch (event.key.key) {
				case SDLK_ESCAPE: exit = true; break;
				case SDLK_T:
					isFullscreen = not isFullscreen; // toggle fullscreen
					SDL_SetWindowFullscreen(sdl.window, isFullscreen);
					break;
				case SDLK_G:
					cameraInteraction = not cameraInteraction;
					CALL_SDL(SDL_SetWindowRelativeMouseMode(sdl.window, cameraInteraction));
					break;
				}
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: resized = true; break;
			case SDL_EVENT_MOUSE_MOTION:
				if (cameraInteraction) camera.rotateBy(event.motion);
				break;
			case SDL_EVENT_MOUSE_WHEEL:
				if (cameraInteraction) camera.zoomBy(event.wheel);
				break;
			}
			ImGui_ImplSDL3_ProcessEvent(&event);
		}

		imguiFrameStart();

		if (cameraInteraction)
			camera.moveBy(scancodeMap[SDL_SCANCODE_W], //
			              scancodeMap[SDL_SCANCODE_S], //
			              scancodeMap[SDL_SCANCODE_A], //
			              scancodeMap[SDL_SCANCODE_D], //
			              scancodeMap[SDL_SCANCODE_R], //
			              scancodeMap[SDL_SCANCODE_F], //
			              deltaTime);

		if (resized) {
			glm::ivec2 newSize = sdl.getWindowSize();
			camera.setWindowSize(newSize);
			glViewport(0, 0, newSize.x, newSize.y);
		}

		ImGui::Checkbox("Show Wireframe", &showWireframe);
		if (showWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(lightVAO);

		shaders.objShader->use();

		shaders.objShader->setDirLight(dirLight);
		shaders.objShader->setPointLights(pointLights);

		ImGui::Checkbox("Display Normals", &displayNormals);

		// move spotlight to camera to act as a flashlight
		Shaders::SpotLight flashlight = baseSpotLight;
		flashlight.position = camera.getPosition();
		flashlight.direction = camera.getFront();
		shaders.objShader->setSpotLight(flashlight);
		shaders.objShader->setDisplayNormals(displayNormals);

		shaders.terrainShader->use();

		shaders.terrainShader->setDirLight(dirLight);
		shaders.terrainShader->setPointLights(pointLights);

		shaders.terrainShader->setDisplayNormals(displayNormals);

		// move spotlight to camera to act as a flashlight
		flashlight = baseSpotLight;
		flashlight.position = camera.getPosition();
		flashlight.direction = camera.getFront();
		shaders.terrainShader->setSpotLight(flashlight);

		std::vector<SceneCascade> stack{};
		recursivelyRender(scene, camera, stack);

		glBindVertexArray(0);
		shaders.objShader->stopUsing();

		for (uint i = 0; i < pointLights.size(); i++) {
			vizualizePointLight(pointLights[i], shaders.lightShader, camera, lightVAO);
		}
		visualizeDirLight(dirLight, shaders.lightShader, camera, lightVAO);

		lastFrameTime = secsSinceInit;
		imguiRender();
		SDL_GL_SwapWindow(sdl.window);
		SDL_Delay(1'000 / 60);
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);

	cleanupImGuiContext();
	sdl.destroy();

	return 0;
}
