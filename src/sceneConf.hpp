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

// this is in a seperate file so changes build faster
std::shared_ptr<SceneGraphRoot> initScene(const ShaderContainer shaders);

#endif /* SCENECONF_HPP */
