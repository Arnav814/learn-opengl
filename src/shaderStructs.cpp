#include "shaderStructs.hpp"

// makes writing shader setters easier
#define SET_UNIFORM_ATTR(attr) \
	do { \
		if (index == NOT_ARRAY) { \
			shader.setUniform(uniformName + "." + #attr, value.attr); \
		} else { \
			shader.setUniform(uniformName + "[" + std::to_string(index) + "]." + #attr, \
			                  value.attr); \
		} \
	} while (false)

void setStructUniform(ShaderProgram& shader, const std::string& uniformName, const Material& value,
                      const int index) {
	SET_UNIFORM_ATTR(diffuseMap);
	SET_UNIFORM_ATTR(specularMap);
	SET_UNIFORM_ATTR(shininess);
}

void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                      const DirectionalLight& value, const int index) {
	SET_UNIFORM_ATTR(direction);
	SET_UNIFORM_ATTR(ambient);
	SET_UNIFORM_ATTR(diffuse);
	SET_UNIFORM_ATTR(specular);
}

void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                      const PointLight& value, const int index) {
	SET_UNIFORM_ATTR(position);
	SET_UNIFORM_ATTR(ambient);
	SET_UNIFORM_ATTR(diffuse);
	SET_UNIFORM_ATTR(specular);
	SET_UNIFORM_ATTR(constant);
	SET_UNIFORM_ATTR(linear);
	SET_UNIFORM_ATTR(quadratic);
}

void setStructUniform(ShaderProgram& shader, const std::string& uniformName, const SpotLight& value,
                      const int index) {
	SET_UNIFORM_ATTR(position);
	SET_UNIFORM_ATTR(direction);
	SET_UNIFORM_ATTR(ambient);
	SET_UNIFORM_ATTR(diffuse);
	SET_UNIFORM_ATTR(specular);
	SET_UNIFORM_ATTR(constant);
	SET_UNIFORM_ATTR(linear);
	SET_UNIFORM_ATTR(quadratic);
	SET_UNIFORM_ATTR(inCutoff);
	SET_UNIFORM_ATTR(outCuttof);
}
