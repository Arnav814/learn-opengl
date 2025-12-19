#include "vertexData.hpp"

#include "lighting.hpp"

std::vector<float> getVertexData() {
	std::vector<float> data{
	    // clang-format off
		 // Back face
		-0.5f, -0.5f, -0.5f, // Bottom-left
		 0.5f,  0.5f, -0.5f, // top-right
		 0.5f, -0.5f, -0.5f, // bottom-right         
		 0.5f,  0.5f, -0.5f, // top-right
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f,  0.5f, -0.5f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f, // bottom-left
		 0.5f, -0.5f,  0.5f, // bottom-right
		 0.5f,  0.5f,  0.5f, // top-right
		 0.5f,  0.5f,  0.5f, // top-right
		-0.5f,  0.5f,  0.5f, // top-left
		-0.5f, -0.5f,  0.5f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f, // top-right
		-0.5f,  0.5f, -0.5f, // top-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f,  0.5f, // bottom-right
		-0.5f,  0.5f,  0.5f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f, // top-left
		 0.5f, -0.5f, -0.5f, // bottom-right
		 0.5f,  0.5f, -0.5f, // top-right         
		 0.5f, -0.5f, -0.5f, // bottom-right
		 0.5f,  0.5f,  0.5f, // top-left
		 0.5f, -0.5f,  0.5f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, // top-right
		 0.5f, -0.5f, -0.5f, // top-left
		 0.5f, -0.5f,  0.5f, // bottom-left
		 0.5f, -0.5f,  0.5f, // bottom-left
		-0.5f, -0.5f,  0.5f, // bottom-right
		-0.5f, -0.5f, -0.5f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f, // top-left
		 0.5f,  0.5f,  0.5f, // bottom-right
		 0.5f,  0.5f, -0.5f, // top-right     
		 0.5f,  0.5f,  0.5f, // bottom-right
		-0.5f,  0.5f, -0.5f, // top-left
		-0.5f,  0.5f,  0.5f, // bottom-left
	    // clang-format on
	};
	return data;
}

std::vector<Shaders::PointLight> getPointLights() {
	LightComponents baseComponents{
	    .ambient = glm::vec3(0.1),
	    .diffuse = glm::vec3(0.5),
	    .specular = glm::vec3(0.5),
	};

	AttenuationComponents baseAttenuation{
	    .constant = 1,
	    .linear = 0.09,
	    .quadratic = 0.032,
	};

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

	std::vector<Shaders::PointLight> data{};
	data.reserve(positions.size());
	for (uint i = 0; i < positions.size(); i++) {
		data.push_back(Shaders::PointLight{
		    .position = positions[i],
			SET_LIGHT_COMPONENTS(baseComponents * colors[i]),
			SET_LIGHT_ATTENUATION(baseAttenuation),
		});
	}

	return data;
}
