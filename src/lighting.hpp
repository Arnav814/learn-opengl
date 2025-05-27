#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "camera.hpp"
#include "common.hpp"
#include "shaders.hpp"
#include "shaderStructs.hpp"

// renders a cube for the purpose of visualizing lights
inline void renderLightCube(ShaderProgram& lightShader, Camera& camera, const uint VAO,
                            const glm::vec3& lightPos, const float scale,
                            const glm::vec3& lightColor) {
	lightShader.use();
	lightShader.setUniform("lightColor", lightColor);

	glm::mat4 obj2world = glm::mat4(1);
	obj2world = glm::translate(obj2world, lightPos);
	obj2world = glm::scale(obj2world, glm::vec3(scale));
	lightShader.setUniform("obj2world", obj2world);
	lightShader.setUniform("world2cam", camera.toCamSpace());
	lightShader.setUniform("projection", camera.projectionMat());

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	lightShader.stopUsing();
}

struct LightComponents {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	LightComponents operator*(const glm::vec3& other) {
		return LightComponents{this->ambient * other, this->diffuse * other,
		                       this->specular * other};
	}

	LightComponents operator*(const float scalar) {
		return LightComponents{this->ambient * scalar, this->diffuse * scalar,
		                       this->specular * scalar};
	}

	LightComponents operator*=(const glm::vec3& other) {
		this->ambient *= other;
		this->diffuse *= other;
		this->specular *= other;
		return *this;
	}

	LightComponents operator*=(const float scalar) {
		this->ambient *= scalar;
		this->diffuse *= scalar;
		this->specular *= scalar;
		return *this;
	}
};

// returns a color that can be used to vizualize the light
glm::vec3 vizualizeLight(const LightComponents& comp);

struct AttenuationComponents {
	float constant;
	float linear;
	float quadratic;

	AttenuationComponents operator*(const AttenuationComponents& b) {
		return AttenuationComponents{this->constant * b.constant, this->linear * b.linear,
		                             this->quadratic * b.quadratic};
	}

	AttenuationComponents operator*=(const float scalar) {
		this->constant *= scalar;
		this->linear *= scalar;
		this->quadratic *= scalar;
		return *this;
	}
};

class DirLight {
  public:
	glm::vec3 direction;
	LightComponents components;

	void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
	                      const int index = NOT_ARRAY) const;

	// vizualize directional lights as a cube 100 units from the player's position
	void vizualize(ShaderProgram& lightShader, Camera& camera, const uint VAO) const {
		renderLightCube(lightShader, camera, VAO, -this->direction * 100.f + camera.getPosition(),
		                1, vizualizeLight(this->components));
	}
};

class PointLight {
  public:
	glm::vec3 position;
	LightComponents components;
	AttenuationComponents attenuation;

	void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
	                      const int index = NOT_ARRAY) const;

	void vizualize(ShaderProgram& lightShader, Camera& camera, const uint VAO) const {
		renderLightCube(lightShader, camera, VAO, this->position, 0.2,
		                vizualizeLight(this->components));
	}
};

class SpotLight {
  public:
	glm::vec3 position;
	glm::vec3 direction;
	LightComponents components;
	AttenuationComponents attenuation;

	// cos of the inside cone's covered angle
	float inCutoff;
	// cos of the outside cone's covered angle
	float outCutoff;

	void setStructUniform(ShaderProgram& shader, const std::string& uniformName,
	                      const int index = NOT_ARRAY) const;

	void vizualize(ShaderProgram& lightShader, Camera& camera, const uint VAO) const {
		renderLightCube(lightShader, camera, VAO, this->position, 0.2,
		                vizualizeLight(this->components));
	}
};

#endif /* LIGHTING_HPP */
