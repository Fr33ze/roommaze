#include "Camera.h"

Camera::Camera(glm::vec3 position)
	: position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(-90.0f), pitch(0.0f), movementSpeed(MOVEMENT_SPEED), mouseSensitivity(MOUSE_SENSITIVITY) {
	
}

Camera::~Camera() {

}

void Camera::updateCameraVectors() {
	// calculate new front vector
	front = glm::normalize(glm::vec3(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
	// also recalculate the right and up vector
	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::processKeyEvent(Key key, float deltaTime) {
	float velocity = movementSpeed * deltaTime;

	switch (key) {
	case(W):
		position += front * velocity;
		break;
	case(S):
		position -= front * velocity;
		break;
	case(A):
		position -= right * velocity;
		break;
	case(D):
		position += right * velocity;
		break;
	}

	// makes sure the user stays at the ground level
	position.y = 0.0f;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// Makes sure that when pitch is out of bounds, screen doesn't get flipped.
	if (constrainPitch) {
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	// update camera vectors using the updated euler angles
	updateCameraVectors();
}

glm::vec3 Camera::getPosition() {
	return position;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}
