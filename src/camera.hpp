#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

struct Camera {
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;

	// the returned matrix tranforms world => camera space
	glm::dmat4 toCamSpace() const {
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
};

#endif /* CAMERA_HPP */
