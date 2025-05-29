#ifndef GENTERRAIN_HPP
#define GENTERRAIN_HPP

#include "common.hpp"
#include "mesh.hpp"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include <boost/multi_array.hpp>

#include <PerlinNoise.hpp>

#include <limits>

class GenTerrain {
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
	GenTerrain(ulong seed, float shininess, glm::vec3 bottomColor, glm::vec3 topColor,
	           glm::vec2 size, glm::uvec2 samples) {
		this->seed = seed;
		this->shininess = shininess;
		this->bottomColor = bottomColor;
		this->topColor = topColor;
		this->size = size;
		this->samples = samples;
		this->noise = siv::PerlinNoise{seed};
	}

	Mesh<ColorVertex> getTerrain();
};

#endif /* GENTERRAIN_HPP */
