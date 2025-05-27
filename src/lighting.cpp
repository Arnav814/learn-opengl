#include "lighting.hpp"

glm::vec3 vizualizeLight(const LightComponents& comp) {
	glm::vec3 color = comp.ambient + comp.diffuse + comp.specular;
	// don't be completely dark, it is a light after all
	color += glm::vec3(0.2);
	color = glm::clamp(color, 0.f, 1.f);
	return color;
}

void DirLight::setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                                const int index) const {
	std::string indexVal = "";
	if (index != NOT_ARRAY) indexVal = "[" + std::to_string(index) + "]";
	shader.setUniform(uniformName + indexVal + ".direction", this->direction);
	shader.setUniform(uniformName + indexVal + ".ambient", this->components.ambient);
	shader.setUniform(uniformName + indexVal + ".diffuse", this->components.diffuse);
	shader.setUniform(uniformName + indexVal + ".specular", this->components.specular);
}

void PointLight::setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                                  const int index) const {
	std::string indexVal = "";
	if (index != NOT_ARRAY) indexVal = "[" + std::to_string(index) + "]";
	shader.setUniform(uniformName + indexVal + ".position", this->position);
	shader.setUniform(uniformName + indexVal + ".ambient", this->components.ambient);
	shader.setUniform(uniformName + indexVal + ".diffuse", this->components.diffuse);
	shader.setUniform(uniformName + indexVal + ".specular", this->components.specular);
	shader.setUniform(uniformName + indexVal + ".constant", this->attenuation.constant);
	shader.setUniform(uniformName + indexVal + ".linear", this->attenuation.linear);
	shader.setUniform(uniformName + indexVal + ".quadratic", this->attenuation.quadratic);
}

void SpotLight::setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                                 const int index) const {
	std::string indexVal = "";
	if (index != NOT_ARRAY) indexVal = "[" + std::to_string(index) + "]";
	shader.setUniform(uniformName + indexVal + ".position", this->position);
	shader.setUniform(uniformName + indexVal + ".direction", this->direction);
	shader.setUniform(uniformName + indexVal + ".ambient", this->components.ambient);
	shader.setUniform(uniformName + indexVal + ".diffuse", this->components.diffuse);
	shader.setUniform(uniformName + indexVal + ".specular", this->components.specular);
	shader.setUniform(uniformName + indexVal + ".constant", this->attenuation.constant);
	shader.setUniform(uniformName + indexVal + ".linear", this->attenuation.linear);
	shader.setUniform(uniformName + indexVal + ".quadratic", this->attenuation.quadratic);
	shader.setUniform(uniformName + indexVal + ".inCutoff", this->inCutoff);
	shader.setUniform(uniformName + indexVal + ".outCutoff", this->outCutoff);
}
