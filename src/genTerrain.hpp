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

class GenTerrain {
  private:
	ulong seed;
	float shininess;
	glm::vec3 bottomColor;
	glm::vec3 topColor;
	glm::vec2 size;
	glm::uvec2 samples;
	siv::PerlinNoise noise;
	std::shared_ptr<ShaderProgram> shader;

	// flatten a 2d coordinate into a 1d index
	// row major
	uint flatten(const glm::uvec2 coord, const glm::uvec2 size) {
		return coord.y * size.x + coord.x;
	}

	glm::vec3 pointFromData(const boost::multi_array<float, 2>& data, const glm::uvec2 point,
	                        const glm::vec2 scale,
	                        const float fallback = std::numeric_limits<float>::quiet_NaN());

  public:
	GenTerrain(const ulong& seed, const float& shininess, const glm::vec3& bottomColor,
	           const glm::vec3& topColor, const glm::vec2& size, const glm::uvec2& samples,
	           const std::shared_ptr<ShaderProgram> shader) {
		this->seed = seed;
		this->shininess = shininess;
		this->bottomColor = bottomColor;
		this->topColor = topColor;
		this->size = size;
		this->samples = samples;
		this->noise = siv::PerlinNoise{seed};
		this->shader = shader;
	}

	Mesh<ColorVertex> getTerrain();
};

#endif /* GENTERRAIN_HPP */
