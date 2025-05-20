#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <SDL3/SDL_events.h>

#include <optional>

// ignores roll
struct EulerAngle {
	float yaw;
	float pitch;

	// normalized direction the angle points
	glm::vec3 toDirection() const {
		return glm::normalize(glm::vec3(cos(this->yaw) * cos(this->pitch), //
		                                sin(this->pitch), //
		                                sin(this->yaw) * cos(this->pitch)));
	}
};

#define WORLD_UP glm::vec3(0, 1, 0)

class Camera {
  private:
	float movementSensitivity; // in distance_units/second
	float rotationSensitivity; // in radians/pixel_moved
	float zoomSensitivity; // in radians/scroll_tick

	glm::vec3 position;
	EulerAngle view;

	float clipNear; // near clipping plane
	float clipFar; // far clipping plane
	glm::ivec2 windowSize; // in pixels
	float zoom; // FOV in radians
	mutable std::optional<glm::dmat4> projectionMatCache;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// should be called after modifying view
	void recalculate();

  public:
	// populates with default values that can later be changed.
	Camera(const glm::ivec2 windowSize);

	void setWindowSize(glm::ivec2 windowSize) {
		this->windowSize = windowSize;
		this->projectionMatCache = std::optional<glm::dmat4>();
	}

	// the returned matrix tranforms world => camera space
	glm::dmat4 toCamSpace() const;

	// get the projection matrix
	glm::dmat4 projectionMat() const;

	// frameTime is multiplied by sensitivity to get the amount to move
	void moveBy(const bool forwards, const bool backwards, const bool left, const bool right,
	            const bool up, const bool down, const float frameTime);

	void setPosition(const glm::vec3& position) { this->position = position; }

	glm::vec3 getPosition() const { return this->position; }

	glm::vec3 getFront() const { return this->front; }

	glm::vec3 getUp() const { return this->up; }

	// frameTime is multiplied by sensitivity to get the amount to rotate
	void rotateBy(const SDL_MouseMotionEvent& event);

	void zoomBy(const SDL_MouseWheelEvent& event);

	// TODO: functions to get/set position, view, etc directly if needed
};

#endif /* CAMERA_HPP */
