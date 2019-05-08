#include "Camera.h"

Camera::Camera(glm::vec3 position, float fieldOfView, float aspectRatio)
	: projectionMatrix(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), groundLevel(position.y), yaw(-90.0f), pitch(0.0f) {
	
	//create physx controller
	extern physx::PxScene* pxScene;
	extern physx::PxPhysics* mPhysics;
	cManager = PxCreateControllerManager(*pxScene);
	physx::PxCapsuleControllerDesc desc;
	desc.height = physx::PxF32(2.0f);
	desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
	desc.contactOffset = physx::PxF32(0.05f);
	desc.stepOffset = physx::PxF32(0.1f);
	desc.radius = physx::PxF32(0.5f);
	desc.position = physx::PxExtendedVec3(position.x, position.y, position.z);
	desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
	desc.reportCallback = this;
	desc.behaviorCallback = this;
	desc.material = mPhysics->createMaterial(1.0f, 1.0f, 0.05);
	controller = cManager->createController(desc);

	filters = physx::PxControllerFilters();

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
	
	physx::PxExtendedVec3 pos = controller->getPosition();
	glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
	shader->setUniform("camera.position", position);
	shader->setUniform("camera.direction", glm::vec3(getViewMatrix()[0][2], getViewMatrix()[1][2], getViewMatrix()[2][2]));
	shader->setUniform("camera.intensity", cameraLight.intensity);
	shader->setUniform("camera.innerCutOff", glm::cos(glm::radians(cameraLight.innerAngle))); // cos()-calculation outside shader (saves valuable time)
	shader->setUniform("camera.outerCutOff", glm::cos(glm::radians(cameraLight.outerAngle)));
	shader->setUniform("camera.attenuation", cameraLight.attenuation);
}

Camera::Camera() {

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
	physx::PxExtendedVec3 pos = controller->getPosition();
	glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyEvent(Key key, bool isRunning, float deltaTime) {
	float velocity = MOVEMENT_SPEED * (isRunning ? 2.0f : 1.0f) * deltaTime;
	physx::PxVec3 disp;
	switch (key) {
	case(W):
		disp = physx::PxVec3(front.x, front.y, front.z) * velocity;
		controller->move(disp, 0.01f, deltaTime, filters);
		break;
	case(S):
		disp = physx::PxVec3(front.x, front.y, front.z) * -velocity;
		controller->move(disp, 0.01f, deltaTime, filters);
		break;
	case(A):
		disp = physx::PxVec3(right.x, right.y, right.z) * -velocity;
		controller->move(disp, 0.01f, deltaTime, filters);
		break;
	case(D):
		disp = physx::PxVec3(right.x, right.y, right.z) * velocity;
		controller->move(disp, 0.01f, deltaTime, filters);
		break;
	}
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
