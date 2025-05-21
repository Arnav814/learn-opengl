#ifndef SHADERSTRUCTS_HPP
#define SHADERSTRUCTS_HPP

#include "shaders.hpp"

#include <glm/vec3.hpp>

struct Material {
	int diffuseMap; // texture id
	int specularMap; // texture id
	float shininess; // specular exponent
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	// for attenuation
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	// for attenuation
	float constant;
	float linear;
	float quadratic;

	// cos of the inside cone's covered angle
	float inCutoff;
	// cos of the outside cone's covered angle
	float outCuttof;
};

// these setters are decoupled from the actual shader class to make updating them easier

void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                             const Material& value);
void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                             const DirectionalLight& value);
void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                             const PointLight& value);
void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
                             const SpotLight& value);

#endif /* SHADERSTRUCTS_HPP */
