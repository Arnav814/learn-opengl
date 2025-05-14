#include "camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <SDL3/SDL_events.h>

#include <optional>

void Camera::recalculate() {
	this->front = glm::normalize(glm::vec3(cos(this->view.yaw) * cos(this->view.pitch), //
	                                       sin(this->view.pitch), //
	                                       sin(this->view.yaw) * cos(this->view.pitch)));
	this->right = glm::normalize(glm::cross(this->front, WORLD_UP));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}

Camera::Camera(const glm::ivec2 windowSize) {
	this->movementSensitivity = 2.5f;
	this->rotationSensitivity = 0.01f;
	this->zoomSensitivity = 0.025f;

	this->position = glm::vec3(0, 0, 0);
	this->view = EulerAngle{glm::radians(-90.f), 0};

	this->clipNear = 0.1;
	this->clipFar = 100;
	this->windowSize = windowSize;
	this->zoom = glm::radians(45.f);
	this->projectionMatCache = std::optional<glm::dmat4>();

	this->recalculate();
}

glm::dmat4 Camera::toCamSpace() const {
	return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::dmat4 Camera::projectionMat() const {
	if (not this->projectionMatCache.has_value())
		this->projectionMatCache =
		    glm::perspective(this->zoom, (float)this->windowSize.x / this->windowSize.y,
		                     this->clipNear, this->clipFar);
	return this->projectionMatCache.value();
}

void Camera::moveBy(const bool forwards, const bool backwards, const bool left, const bool right,
                    const bool up, const bool down, const float frameTime) {

	float scaledMovementSpeed = this->movementSensitivity * frameTime;

	// front without the y componenent
	glm::vec3 flatFront = this->front;
	flatFront.y = 0;
	flatFront = glm::normalize(flatFront);

	if (forwards) this->position += flatFront * scaledMovementSpeed;
	if (backwards) this->position -= flatFront * scaledMovementSpeed;
	if (right) this->position += this->right * scaledMovementSpeed;
	if (left) this->position -= this->right * scaledMovementSpeed;
	if (up) this->position += WORLD_UP * scaledMovementSpeed;
	if (down) this->position -= WORLD_UP * scaledMovementSpeed;
}

void Camera::rotateBy(const SDL_MouseMotionEvent& event) {
	this->view.yaw += (float)event.xrel * this->rotationSensitivity;
	this->view.pitch -= (float)event.yrel * this->rotationSensitivity;

	// make sure we don't look directly up or down because
	// looking over 90° causes the view to flip upside down
	if (this->view.pitch > glm::radians(89.f)) this->view.pitch = glm::radians(89.f);
	if (this->view.pitch < glm::radians(-89.f)) this->view.pitch = glm::radians(-89.f);

	this->recalculate();
}

void Camera::zoomBy(const SDL_MouseWheelEvent& event) {
	this->zoom += event.y * this->zoomSensitivity;

	// keep zoom between 1° and 180°
	if (this->zoom < glm::radians(1.f)) this->zoom = glm::radians(1.f);
	if (this->zoom > glm::radians(180.f)) this->zoom = glm::radians(180.f);

	this->projectionMatCache = std::optional<glm::dmat4>();
}
