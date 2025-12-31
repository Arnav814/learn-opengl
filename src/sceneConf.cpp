#include "sceneConf.hpp"

#include "genTerrain.hpp"
#include "model.hpp"

#include <boost/program_options.hpp>

#include <iostream>

std::shared_ptr<Config> parseArgs(const int argc, const char* const* const argv) {
	namespace po = boost::program_options;

	po::options_description desc(argv[0]);
	desc.add_options() //
	    ("help", "Print help message") //
	    ("no-models", "Don't load any models") //
	    ("no-terrain", "Don't load any terrain"); //

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return NULL;
	}

	Config conf = {
	    .loadModels = !vm.count("no-models"),
	    .loadTerrain = !vm.count("no-terrain"),
	};

	return std::make_shared<Config>(conf);
}

std::shared_ptr<SceneGraphRoot> initScene(const ShaderContainer shaders, const Config& config) {
	std::shared_ptr<SceneGraphRoot> scene = std::make_shared<SceneGraphRoot>();

	// MODELS

	if (config.loadModels) {
		std::print("Loading models... ");
		std::fflush(stdout);

		scene->addChild(std::shared_ptr<Model>(
		    new Model{MEDIA_DIR "./backpack/backpack.obj", shaders.objShader}));

		std::println("Done.");
	}

	// TERRAIN

	if (config.loadTerrain) {
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
	}

	std::vector<SceneCascade> stack{};
	recursivelyPrint(scene, stack);

	return scene;
}
