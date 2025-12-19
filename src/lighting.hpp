#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "camera.hpp"
#include "common.hpp"
#include "lightCube.hpp"
#include "object.hpp"
#include "shaders.hpp"
#include "shaderStructs.hpp"

// renders a cube for the purpose of visualizing lights
inline void renderLightCube(Shaders::LightCube lightShader, Camera& camera, const uint VAO,
                            const glm::vec3& lightPos, const float scale,
                            const glm::vec3& lightColor) {
	lightShader->use();
	lightShader->setLightColor(lightColor);

	glm::mat4 obj2world = glm::mat4(1);
	obj2world = glm::translate(obj2world, lightPos);
	obj2world = glm::scale(obj2world, glm::vec3(scale));
	lightShader->setObj2world(obj2world);
	lightShader->setWorld2cam(camera.toCamSpace());
	lightShader->setProjection(camera.projectionMat());

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	lightShader->stopUsing();
}

struct LightComponents {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	LightComponents operator*(const glm::vec3& other) {
		return {this->ambient * other, this->diffuse * other, this->specular * other};
	}

	LightComponents operator*(const float scalar) {
		return {this->ambient * scalar, this->diffuse * scalar, this->specular * scalar};
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

// to be used in named init lists
#define SET_LIGHT_COMPONENTS(comp) \
	.ambient = (comp).ambient, .diffuse = (comp).diffuse, .specular = (comp).specular

template <typename T> LightComponents getComponents(const T& light) {
	return {light.ambient, light.diffuse, light.specular};
}

// returns a color that can be used to vizualize the light
glm::vec3 vizualizeLight(const LightComponents& comp);

struct AttenuationComponents {
	float constant;
	float linear;
	float quadratic;

	AttenuationComponents operator*(const AttenuationComponents& b) {
		return {this->constant * b.constant, this->linear * b.linear,
		        this->quadratic * b.quadratic};
	}

	AttenuationComponents operator*=(const float scalar) {
		this->constant *= scalar;
		this->linear *= scalar;
		this->quadratic *= scalar;
		return *this;
	}
};

#define SET_LIGHT_ATTENUATION(att) \
	.constant = att.constant, .linear = att.linear, .quadratic = att.quadratic

template <typename T> AttenuationComponents getAttenuation(const T& light) {
	return {light.constant, light.linear, light.quadratic};
}

inline void visualizeDirLight(const Shaders::DirectionalLight& light, Shaders::LightCube shader,
                              Camera& camera, const uint VAO) {
	renderLightCube(shader, camera, VAO, -light.direction * 100.f + camera.getPosition(), 1,
	                vizualizeLight(getComponents(light)));
}

inline void vizualizePointLight(const Shaders::PointLight& light, Shaders::LightCube& shader,
                                Camera& camera, const uint VAO) {
	renderLightCube(shader, camera, VAO, light.position, 0.2, vizualizeLight(getComponents(light)));
}

inline void vizualize(const Shaders::SpotLight& light, Shaders::LightCube& shader, Camera& camera,
                      const uint VAO) {
	renderLightCube(shader, camera, VAO, light.position, 0.2, vizualizeLight(getComponents(light)));
}

#endif /* LIGHTING_HPP */
