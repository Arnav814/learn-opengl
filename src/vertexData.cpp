#include "vertexData.hpp"

std::vector<float> getVertexData() {
	std::vector<float> data{
	    // clang-format off
		// vertexes        || normals           || texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	    // clang-format on
	};
	return data;
}

std::vector<glm::vec3> getCubePositions() {
	std::vector<glm::vec3> data{
	    glm::vec3(0.0f, 0.0f, 0.0f), //
	    glm::vec3(2.0f, 5.0f, -15.0f), //
	    glm::vec3(-1.5f, -2.2f, -2.5f), //
	    glm::vec3(-3.8f, -2.0f, -12.3f), //
	    glm::vec3(2.4f, -0.4f, -3.5f), //
	    glm::vec3(-1.7f, 3.0f, -7.5f), //
	    glm::vec3(1.3f, -2.0f, -2.5f), //
	    glm::vec3(1.5f, 2.0f, -2.5f), //
	    glm::vec3(1.5f, 0.2f, -1.5f), //
	    glm::vec3(-1.3f, 1.0f, -1.5f) //
	};
	return data;
}

std::vector<PointLight> getPointLights() {
	float baseAmbient = 0.1;
	float baseDiffuse = 0.5;
	float baseSpecular = 0.5;

	std::vector<glm::vec3> colors{
	    glm::vec3(1, 0.5, 0.5), //
	    glm::vec3(1, 1, 1), //
	    glm::vec3(0, 0.6, 1), //
	    glm::vec3(1, 0.5, 1), //
	};

	std::vector<glm::vec3> positions{
	    glm::vec3(0.7f, 0.2f, 2.0f), //
	    glm::vec3(2.3f, -3.3f, -4.0f), //
	    glm::vec3(-4.0f, 2.0f, -12.0f), //
	    glm::vec3(0.0f, 0.0f, -3.0f) //
	};

	std::vector<PointLight> data{};
	data.reserve(positions.size());
	for (uint i = 0; i < positions.size(); i++) {
		data.push_back(PointLight{
		    .position = positions[i],
		    .ambient = colors[i] * baseAmbient,
		    .diffuse = colors[i] * baseDiffuse,
		    .specular = colors[i] * baseSpecular,
		    .constant = 1,
		    .linear = 0.09,
		    .quadratic = 0.032,
		});
	}

	return data;
}
