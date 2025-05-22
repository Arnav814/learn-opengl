#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "camera.hpp"
#include "common.hpp"
#include "shaders.hpp"

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

// returns a color that can be used to vizualize the light
template <typename LightType> inline glm::vec3 vizualizeLight(const LightType& light) {
	glm::vec3 color = light.ambient + light.diffuse + light.specular;
	// don't be completely dark, it is a light after all
	color += glm::vec3(0.2);
	color = glm::clamp(color, 0.f, 1.f);
	return color;
}

#endif /* LIGHTING_HPP */
