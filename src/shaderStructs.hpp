#ifndef SHADERSTRUCTS_HPP
#define SHADERSTRUCTS_HPP

#include "shaders.hpp"

#include <glm/vec3.hpp>

// these setters are decoupled from the actual shader class to make updating them easier

// makes writing shader setters easier
#define SET_UNIFORM_ATTR(attr) \
	do { \
		shader.setUniform(uniformName + "." + #attr, value.attr); \
	} while (false)

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess; // specular exponent
};

inline void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                             const Material& value) {
	SET_UNIFORM_ATTR(ambient);
	SET_UNIFORM_ATTR(diffuse);
	SET_UNIFORM_ATTR(specular);
	SET_UNIFORM_ATTR(shininess);
}

#endif /* SHADERSTRUCTS_HPP */
