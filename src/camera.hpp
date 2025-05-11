#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// ignores roll
struct EulerAngle {
	float yaw;
	float pitch;

	// normalized direction the angle points
	glm::vec3 toDirection() const {
		return glm::vec3(cos(this->yaw) * cos(this->pitch), sin(this->pitch),
		                 sin(this->yaw) * cos(this->pitch));
	}
};

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
