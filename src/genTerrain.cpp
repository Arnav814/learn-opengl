#include "genTerrain.hpp"

glm::vec3 Terrain::pointFromData(const boost::multi_array<float, 2>& data,
                                    const glm::uvec2 point, const glm::vec2 scale,
                                    const float fallback) {
	float value;
	if (point.x >= 0 and point.x < data.shape()[0] and point.y >= 0 and point.y < data.shape()[1])
		value = data[point.x][point.y];
	else value = fallback;
	return glm::vec3(point.x * scale.x, value, point.y * scale.y);
}

Mesh<ColorVertex> Terrain::getTerrain() {
	glm::vec2 scale = glm::vec2(this->size.x / this->samples.x, this->size.y / this->samples.y);
	boost::multi_array<float, 2> terrainData(boost::extents[this->samples.x][this->samples.y]);

	for (uint y = 0; y < this->samples.y; y++) {
		for (uint x = 0; x < this->samples.x; x++) {
			terrainData[x][y] = this->noise.noise2D_01(x * scale.x, y * scale.y);
		}
	}

	std::vector<ColorVertex> verticies;

	for (uint y = 0; y < this->samples.y; y++) {
		for (uint x = 0; x < this->samples.x; x++) {
			glm::vec3 position = this->pointFromData(terrainData, {x, y}, scale);

			// stored clockwise
			std::array<glm::vec3, 4> nearbyPoints{
			    // fallback to this position's height to make corners mostly work
			    this->pointFromData(terrainData, {x + 1, y}, scale, position.y),
			    this->pointFromData(terrainData, {x, y - 1}, scale, position.y),
			    this->pointFromData(terrainData, {x - 1, y}, scale, position.y),
			    this->pointFromData(terrainData, {x, y + 1}, scale, position.y),
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
			    std::accumulate(ALL_OF(normalsWith), glm::vec3(0)) / glm::vec3(normalsWith.size());
			glm::vec3 color = glm::mix(this->bottomColor, this->topColor, position.y);

			verticies.push_back(ColorVertex{
			    .position = position,
			    .normal = averageNormal, // TODO: fix artifacts in normals with strange X patterns
			    .color = color,
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

	return Mesh<ColorVertex>{verticies, indicies, this->shininess, this->shader};
}
