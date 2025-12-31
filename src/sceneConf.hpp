#ifndef SCENECONF_HPP
#define SCENECONF_HPP

#include "lightCube.hpp"
#include "object.hpp"
#include "sceneObject.hpp"
#include "terrain.hpp"

#include <memory>

struct ShaderContainer {
	Shaders::Object objShader;
	Shaders::Terrain terrainShader;
	Shaders::LightCube lightShader;
};

struct Config {
	bool loadModels; // really slow, skipping makes init faster
	bool loadTerrain;
};

// may return null to indicate the user only wanted help text, version, etc
std::shared_ptr<Config> parseArgs(const int argc, const char* const * const argv);

// this is in a seperate file so changes build faster
std::shared_ptr<SceneGraphRoot> initScene(const ShaderContainer shaders, const Config& config);

#endif /* SCENECONF_HPP */
