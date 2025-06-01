#ifndef GENTERRAIN_HPP
#define GENTERRAIN_HPP

#include "common.hpp"
#include "mesh.hpp"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include <boost/multi_array.hpp>

#include <PerlinNoise.hpp>

#include <limits>
#include <memory>

class Terrain : public BaseSceneGraphObject {
  private:
	ulong seed;
	float shininess;
	glm::vec3 bottomColor;
	glm::vec3 topColor;
	glm::vec2 size;
	glm::uvec2 samples;
	siv::PerlinNoise noise;

	// flatten a 2d coordinate into a 1d index
	// row major
	uint flatten(const glm::uvec2 coord, const glm::uvec2 size) {
		return coord.y * size.x + coord.x;
	}

	glm::vec3 pointFromData(const boost::multi_array<float, 2>& data, const glm::uvec2 point,
	                        const glm::vec2 scale,
	                        const float fallback = std::numeric_limits<float>::quiet_NaN());

  public:
	Terrain(const ulong& seed, const float& shininess, const glm::vec3& bottomColor,
	        const glm::vec3& topColor, const glm::vec2& size, const glm::uvec2& samples,
	        const ShaderPtr shader)
	    : BaseSceneGraphObject(shader, glm::mat4(1)) {
		this->seed = seed;
		this->shininess = shininess;
		this->bottomColor = bottomColor;
		this->topColor = topColor;
		this->size = size;
		this->samples = samples;
		this->noise = siv::PerlinNoise{seed};

		Mesh<ColorVertex> terrain = this->getTerrain();
		this->addChild(std::make_shared<Mesh<ColorVertex>>(terrain));
	}

	virtual void render(const Camera& camera [[maybe_unused]],
	                    const SceneCascade& cascade [[maybe_unused]]) {}

	virtual void print(const SceneCascade& cascade) {
		std::println("{}Terrain:", std::string(SCENE_GRAPH_INDENT * cascade.recurseDepth, ' '));
	}

	Mesh<ColorVertex> getTerrain();
};

#endif /* GENTERRAIN_HPP */
