#include "camera.hpp"
#include "common.hpp"
#include "lighting.hpp"
#include "loadTexture.hpp"
#include "shaders.hpp"
#include "shaderStructs.hpp"
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

	const DirectionalLight dirLight{
	    .direction = glm::normalize(glm::vec3(0.1, -1, 0.1)),
	    .ambient = glm::vec3(.1),
	    .diffuse = glm::vec3(.5),
	    .specular = glm::vec3(.5),
	};

	std::vector<PointLight> pointLights = getPointLights();

	glm::vec3 spotLightColor = glm::vec3(1, 1, 0.8); // yellowish
	// translate to the appropriate position before use
	const SpotLight baseSpotLight{
	    .position = origin3d, // placeholder
	    .direction = origin3d, // placeholder
	    .ambient = spotLightColor * 0.1f,
	    .diffuse = spotLightColor * 0.5f,
	    .specular = spotLightColor * 0.5f,
	    .constant = 1.0,
	    .linear = 0.09,
	    .quadratic = 0.032,
	    .inCutoff = glm::cos(glm::radians(12.5f)),
	    .outCuttof = glm::cos(glm::radians(17.5f)),

	};

	// SHADERS

	ShaderProgram objShader{SOURCE_DIR "./shaders/object.vert.glsl",
	                        SOURCE_DIR "./shaders/object.frag.glsl"};
	ShaderProgram lightShader{SOURCE_DIR "./shaders/lightCube.vert.glsl",
	                          SOURCE_DIR "./shaders/lightCube.frag.glsl"};

	// TEXTURES

	uint diffuseTexId = loadTexture("../media/container2.jpg");
	uint specularTexId = loadTexture("../media/container2_specular.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexId);

	// CONTAINER BUFFERS

	std::vector<float> verticies = getVertexData();
	std::vector<glm::vec3> cubePositions = getCubePositions();

	uint vertBufObj;
	uint objVertAttribObj;
	glGenVertexArrays(1, &objVertAttribObj);
	glGenBuffers(1, &vertBufObj);
	glBindVertexArray(objVertAttribObj);

	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(),
	             GL_STATIC_DRAW);

	uint stepSize = (3 + 3 + 2) * sizeof(float);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)0);
	glEnableVertexAttribArray(0);

	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stepSize, (void*)((3 + 3) * sizeof(float)));
	glEnableVertexAttribArray(2);

	// LIGHT BUFFERS

	uint lightVertAttribObj;
	glGenVertexArrays(1, &lightVertAttribObj);
	glBindVertexArray(lightVertAttribObj);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);

	// only position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)0);
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
			SDL_GetWindowSizeInPixels(window, &width, &height);
			camera.setWindowSize(glm::ivec2(width, height));
			glViewport(0, 0, width, height);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(objVertAttribObj);

		objShader.use();
		objShader.setUniform("world2cam", camera.toCamSpace());
		objShader.setUniform("projection", camera.projectionMat());
		objShader.setUniform("viewPos", camera.getPosition());

		setStructUniform(objShader, "dirLight", dirLight);

		for (uint i = 0; i < pointLights.size(); i++) {
			setStructUniform(objShader, "pointLights", pointLights[i], i);
		}

		// move spotlight to camera to act as a flashlight
		SpotLight flashlight = baseSpotLight;
		flashlight.position = camera.getPosition();
		flashlight.direction = camera.getFront();
		setStructUniform(objShader, "spotLight", flashlight);

		setStructUniform(objShader, "material",
		                 Material{.diffuseMap = 0, .specularMap = 1, .shininess = 32});

		float angle = 0; // in radians
		for (const glm::vec3& cubePos : cubePositions) {
			obj2world = glm::mat4(1);
			obj2world = glm::translate(obj2world, cubePos);
			obj2world = glm::rotate(obj2world, angle, glm::normalize(glm::vec3(1, 2, 3)));
			objShader.setUniform("obj2world", obj2world);
			objShader.setUniform("obj2normal", glm::mat3(glm::transpose(glm::inverse(obj2world))));
			glDrawArrays(GL_TRIANGLES, 0, 36);
			angle++;
		}

		glBindVertexArray(0);
		objShader.stopUsing();

		for (uint i = 0; i < pointLights.size(); i++) {
			renderLightCube(lightShader, camera, lightVertAttribObj, pointLights[i].position, 0.2,
			                vizualizeLight(pointLights[i]));
		}
		// vizualize directional lights as a cube 100 units from the player's position
		renderLightCube(lightShader, camera, lightVertAttribObj,
		                -dirLight.direction * 100.f + camera.getPosition(), 1,
		                vizualizeLight(dirLight));

		lastFrameTime = secsSinceInit;
		SDL_GL_SwapWindow(window);
		SDL_Delay(1'000 / 60);
	}

	glDeleteVertexArrays(1, &objVertAttribObj);
	glDeleteBuffers(1, &vertBufObj);
	objShader.~ShaderProgram();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
