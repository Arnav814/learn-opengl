#include "sceneConf.hpp"

#include "genTerrain.hpp"
#include "model.hpp"

std::shared_ptr<SceneGraphRoot> initScene(const ShaderContainer shaders) {
	// MODELS

	std::print("Loading models... ");
	std::fflush(stdout);

	std::shared_ptr<SceneGraphRoot> scene = std::make_shared<SceneGraphRoot>();
	scene->addChild(
	    std::shared_ptr<Model>(new Model{MEDIA_DIR "./backpack/backpack.obj", shaders.objShader}));

	std::println("Done.");

	// TERRAIN

	std::print("Generating terrain... ");
	std::fflush(stdout);
	constexpr DSColor grass{
	    glm::vec3(0.96, 0.84, 0.69),
	    glm::vec3(0.96, 0.84, 0.69) / 16.f,
	};
	constexpr DSColor sand{
	    glm::vec3(0.25, 0.60, 0.04),
	    glm::vec3(0.25, 0.60, 0.04) / 4.f,
	};
	Terrain terrain{
	    123'123, 32, grass, sand, glm::vec3(5, 5, 1), glm::ivec2(25), shaders.terrainShader};
	terrain.setTransform(glm::translate(glm::identity<glm::mat4>(), {5, 0, 0}));
	scene->addChild(std::make_shared<Terrain>(terrain));
	std::println("Done.");

	std::vector<SceneCascade> stack{};
	recursivelyPrint(scene, stack);

	return scene;
}
