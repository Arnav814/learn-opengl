#include "lighting.hpp"

glm::vec3 vizualizeLight(const LightComponents& comp) {
	glm::vec3 color = comp.ambient + comp.diffuse + comp.specular;
	// don't be completely dark, it is a light after all
	color += glm::vec3(0.2);
	color = glm::clamp(color, 0.f, 1.f);
	return color;
}

