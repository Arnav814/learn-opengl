#include "genTerrain.hpp"

#include "glmFormatters.hpp"

#include <glm/geometric.hpp>

glm::vec3 Terrain::pointFromData(const boost::multi_array<float, 2>& data, const glm::uvec2 point,
                                 const glm::vec3 scale, const float fallback) {
	float value;
	if (point.x >= 0 and point.x < data.shape()[0] and point.y >= 0 and point.y < data.shape()[1])
		value = data[point.x][point.y];
	else value = fallback;
	return {point.x * scale.x, value, point.y * scale.y};
}

glm::vec3 Terrain::pointAt(const glm::vec2& pos) {
	return {pos.x, this->noise.noise2D_01(pos.x, pos.y) * this->size.z, pos.y};
}

Mesh<ColorVertex, Shaders::Terrain> Terrain::getTerrain() {
	glm::vec3 scale = {this->size.x / this->samples.x, this->size.y / this->samples.y,
	                   this->size.z};
	boost::multi_array<float, 2> terrainData(boost::extents[this->samples.x][this->samples.y]);
	boost::multi_array<glm::vec3, 2> terrainNormals(
	    boost::extents[this->samples.x][this->samples.y]);

	for (uint y = 0; y < this->samples.y; y++) {
		for (uint x = 0; x < this->samples.x; x++) {
			glm::vec3 position = this->pointAt({x * scale.x, y * scale.y});
			terrainData[x][y] = position.y;
			// a small number
			float small = fmin(this->size.x / this->samples.x, this->size.y / this->samples.y) / 10;

			// stored clockwise
			std::array<glm::vec3, 4> nearbyPoints{
			    this->pointAt({position.x + small, position.z}),
			    this->pointAt({position.x, position.z - small}),
			    this->pointAt({position.x - small, position.z}),
			    this->pointAt({position.x, position.z + small}),
			};

			// normalized vectors from this position to each nearby point
			std::array<glm::vec3, 4> toEachPoint{
			    glm::normalize(position - nearbyPoints[0]),
			    glm::normalize(position - nearbyPoints[1]),
			    glm::normalize(position - nearbyPoints[2]),
			    glm::normalize(position - nearbyPoints[3]),
			};

			// compute normals for each quadrant
			std::array<glm::vec3, 4> normalsWith{
			    glm::cross(toEachPoint[0], toEachPoint[1]),
			    glm::cross(toEachPoint[1], toEachPoint[2]),
			    glm::cross(toEachPoint[2], toEachPoint[3]),
			    glm::cross(toEachPoint[3], toEachPoint[0]),
			};

			// average the array
			glm::vec3 averageNormal =
			    glm::normalize(std::accumulate(ALL_OF(normalsWith), glm::vec3(0)));
			terrainNormals[x][y] = averageNormal;
		}
	}

	std::vector<ColorVertex> verticies;

	for (uint y = 0; y < this->samples.y; y++) {
		for (uint x = 0; x < this->samples.x; x++) {
			glm::vec3 position = this->pointFromData(terrainData, {x, y}, scale);
			glm::vec3 normal = terrainNormals[x][y];

			glm::vec3 diffuse =
			    glm::mix(this->bottomColor.diffuse, this->topColor.diffuse, position.y / scale.z);
			glm::vec3 specular =
			    glm::mix(this->bottomColor.specular, this->topColor.specular, position.y / scale.z);

			verticies.push_back(ColorVertex{
			    .position = position,
			    .normal = normal, // TODO: fix artifacts in normals with strange X patterns
			    .diffuse = diffuse,
			    .specular = specular,
			});
		}
	}

	std::vector<uint> indicies;

	// get triangle indicies by going square by square across the data
	for (uint y = 0; y < this->samples.y - 1; y++) {
		for (uint x = 0; x < this->samples.x - 1; x++) {
			// first half of square
			indicies.push_back(this->flatten({x, y + 1}, this->samples));
			indicies.push_back(this->flatten({x + 1, y}, this->samples));
			indicies.push_back(this->flatten({x, y}, this->samples));
			// second half of square
			indicies.push_back(this->flatten({x, y + 1}, this->samples));
			indicies.push_back(this->flatten({x + 1, y + 1}, this->samples));
			indicies.push_back(this->flatten({x + 1, y}, this->samples));
		}
	}

	return Mesh<ColorVertex, Shaders::Terrain>{verticies, indicies, this->shininess, this->shader};
}
