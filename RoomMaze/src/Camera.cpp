#include "Camera.h"

Camera::Camera(glm::vec3 position, float fieldOfView, float aspectRatio)
	: projectionMatrix(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), groundLevel(position.y), yaw(-90.0f), pitch(0.0f) {
	
	displacement = physx::PxVec3(0.0f, 0.0f, 0.0f);

	//create physx controller
	extern physx::PxScene* pxScene;
	extern physx::PxPhysics* mPhysics;
	cManager = PxCreateControllerManager(*pxScene);
	physx::PxCapsuleControllerDesc desc;
	desc.height = physx::PxF32(1.0f);
	desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
	desc.contactOffset = physx::PxF32(0.05f);
	desc.stepOffset = physx::PxF32(0.15f);
	desc.radius = physx::PxF32(0.15f);
	desc.position = physx::PxExtendedVec3(position.x, position.y, position.z);
	desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
	cctCallbacks = new CharacterCallback();
	desc.reportCallback = cctCallbacks;
	desc.behaviorCallback = cctCallbacks;
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
	glm::vec3 position = glm::vec3(pos.x, pos.y + 1, pos.z);
	shader->setUniform("camera.position", position);
	glm::mat4 viewmat = getViewMatrix();
	shader->setUniform("camera.direction", glm::vec3(viewmat[0][2], viewmat[1][2], viewmat[2][2]));
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
	glm::vec3 position = glm::vec3(pos.x, pos.y + 1, pos.z);
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyEvent(Key key, bool isRunning, float deltaTime) {
	float velocity = MOVEMENT_SPEED * (isRunning ? 2.0f : 1.0f) * deltaTime;
	switch (key) {
	case(W):
		displacement += physx::PxVec3(front.x, 0, front.z).getNormalized() * velocity;
		break;
	case(S):
		displacement += physx::PxVec3(front.x, 0, front.z).getNormalized() * -velocity;
		break;
	case(A):
		displacement += physx::PxVec3(right.x, 0, right.z).getNormalized() * -velocity;
		break;
	case(D):
		displacement += physx::PxVec3(right.x, 0, right.z).getNormalized() * velocity;
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

void Camera::move(float deltaTime)
{
	// gravity
	displacement.y = -5.0f * deltaTime;

	// movement
	controller->move(displacement, 0.01f, deltaTime, filters);

	// reset vector
	displacement.x = 0.0f;
	displacement.z = 0.0f;
}
