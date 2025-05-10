#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

struct Camera {
	glm::vec3 position;
	glm::vec3 target; // a point the camera should point at

	// normalized vector from position to target
	// reverse direction of the camera (+z axis)
	// +z points into the camera, -z points away
	glm::vec3 targetToPos() const { return glm::normalize(this->position - this->target); }

	// points in the -x axis
	glm::vec3 right() const {
		return glm::normalize(glm::cross(glm::vec3(0, 1, 0), this->targetToPos()));
	}

	// points in the +y axis
	glm::vec3 up() const {
		// inputs are already normalized, no need to do it again
		return glm::cross(this->targetToPos(), this->right());
	}

	// the returned matrix tranforms world => camera space
	glm::dmat4 toCamSpace() const { return glm::lookAt(this->position, this->target, this->up()); }
};

#endif /* CAMERA_HPP */
