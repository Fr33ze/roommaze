#include "Camera.h"

Camera::Camera(glm::vec3 position, float fieldOfView, float aspectRatio)
	: position(position), projectionMatrix(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(-90.0f), pitch(0.0f) {
	
	updateCameraVectors();
}

void Camera::setSpotLightParameters(glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation) {
	cameraLight.intensity = intensity;
	cameraLight.innerAngle = innerAngle;
	cameraLight.outerAngle = outerAngle;
	cameraLight.attenuation = attenuation;
}

void Camera::setUniforms(std::shared_ptr<Shader> shader) {
	shader->setUniform("viewMatrix", getViewMatrix());
	shader->setUniform("projectionMatrix", projectionMatrix);
	
	shader->setUniform("camera.position", position);
	shader->setUniform("camera.direction", glm::vec3(getViewMatrix()[0][2], getViewMatrix()[1][2], getViewMatrix()[2][2]));
	shader->setUniform("camera.intensity", cameraLight.intensity);
	shader->setUniform("camera.innerCutOff", glm::cos(glm::radians(cameraLight.innerAngle))); // cos()-calculation outside shader (saves valuable time)
	shader->setUniform("camera.outerCutOff", glm::cos(glm::radians(cameraLight.outerAngle)));
	shader->setUniform("camera.attenuation", cameraLight.attenuation);
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

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyEvent(Key key, bool isRunning, float deltaTime) {
	float velocity = MOVEMENT_SPEED * (isRunning ? 2.0f : 1.0f) * deltaTime;

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

	// user stays at ground level (no flying camery system)
	position.y = 0.0f;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= MOUSE_SENSITIVITY;
	yOffset *= MOUSE_SENSITIVITY;

	yaw += xOffset;
	pitch += yOffset;

	// when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	// update camera vectors using the updated euler angles
	updateCameraVectors();
}
