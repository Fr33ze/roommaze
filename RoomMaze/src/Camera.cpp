#include "Camera.h"

Camera::Camera(glm::vec3 position, float fieldOfView, float aspectRatio)
	: projectionMatrix(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(-90.0f), pitch(0.0f) {

	displacement = physx::PxVec3(0.0f, 0.0f, 0.0f);

	alListener3f(AL_POSITION, position.x, position.y, position.z);
	alListener3f(AL_VELOCITY, position.x + front.x, position.y + front.y, position.z + front.z);

	for (int i = 0; i < 5; i++) {
		concreteBuffers[i] = alutCreateBufferFromFile(("assets/audio/concrete_steps/" + std::to_string(i) + ".wav").c_str());
		waterBuffers[i] = alutCreateBufferFromFile(("assets/audio/water_steps/" + std::to_string(i) + ".wav").c_str());
	}
	neonBuffer = alutCreateBufferFromFile("assets/audio/neontube_hum.wav");

	alGenSources(2, audioSources);
	alSourcef(audioSources[0], AL_PITCH, 1);
	alSourcef(audioSources[0], AL_GAIN, 1);
	alSourcei(audioSources[0], AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(audioSources[0], AL_POSITION, 0, -CHARACTER_EYE_HEIGHT, 0);
	alSourcef(audioSources[1], AL_PITCH, 1);
	alSourcef(audioSources[1], AL_GAIN, 1);
	alSourcei(audioSources[1], AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(audioSources[1], AL_POSITION, 0, -CHARACTER_EYE_HEIGHT, 0);
	
	alGenSources(1, &neonSource);
	alSourcef(neonSource, AL_PITCH, 1);
	alSourcef(neonSource, AL_GAIN, 1);
	alSource3f(neonSource, AL_POSITION, -6.0f, 1.8f, 0.0f);
	alSourcei(neonSource, AL_BUFFER, neonBuffer);
	alSourcei(neonSource, AL_LOOPING, AL_TRUE);

	//create physx controller
	extern physx::PxScene* pxScene;
	extern physx::PxPhysics* mPhysics;
	cManager = PxCreateControllerManager(*pxScene);
	physx::PxCapsuleControllerDesc desc;
	desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
	desc.radius = physx::PxF32(CHARACTER_RADIUS);
	desc.contactOffset = physx::PxF32(0.005f);
	desc.height = physx::PxF32(CHARACTER_HEIGHT - CHARACTER_RADIUS * 2 - desc.contactOffset * 2);
	desc.stepOffset = physx::PxF32(0.25f);
	desc.position = physx::PxExtendedVec3(position.x, position.y, position.z);
	desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
	cctCallbacks = new CharacterCallback();
	desc.reportCallback = this;
	desc.behaviorCallback = cctCallbacks;
	desc.material = mPhysics->createMaterial(physx::PxReal(1.0f), physx::PxReal(1.0f), physx::PxReal(0.05));
	controller = cManager->createController(desc);

	filters = physx::PxControllerFilters();
	filters.mFilterData = new physx::PxFilterData(COLLISION, 0, 0, 0);

	updateCameraVectors();

	cameraLight.isTurnedOn = true;

	distr = std::uniform_int_distribution<>(0, 4);
	alSourcei(audioSources[0], AL_BUFFER, concreteBuffers[distr(generator)]);
}

void Camera::setSpotLightParameters(float brightness, glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation) {
	cameraLight.brightness = brightness;
	cameraLight.intensity = intensity;
	cameraLight.innerAngle = innerAngle;
	cameraLight.outerAngle = outerAngle;
	cameraLight.attenuation = attenuation;
}

void Camera::setUniforms(std::shared_ptr<Shader> shader) {
	setLights(shader);

	shader->setUniform("viewMatrix", getViewMatrix());
	shader->setUniform("projectionMatrix", projectionMatrix);

	physx::PxExtendedVec3 pos = controller->getFootPosition();
	glm::vec3 position = glm::vec3(pos.x, pos.y + 1.75f, pos.z);
	shader->setUniform("camera.position", position);
	shader->setUniform("camera.brightnessModifier", brightnessModifier < 0 ? 0.0f : 1.0f);
	glm::mat4 viewmat = getViewMatrix();
	shader->setUniform("camera.direction", glm::vec3(viewmat[0][2], viewmat[1][2], viewmat[2][2]));
	shader->setUniform("camera.brightness", cameraLight.brightness);
	shader->setUniform("camera.intensity", cameraLight.isTurnedOn ? cameraLight.intensity : glm::vec3(0.0f));
	shader->setUniform("camera.innerCutOff", glm::cos(glm::radians(cameraLight.innerAngle))); // cos()-calculation outside shader (saves valuable time)
	shader->setUniform("camera.outerCutOff", glm::cos(glm::radians(cameraLight.outerAngle)));
	shader->setUniform("camera.attenuation", cameraLight.attenuation);
}

void Camera::setLights(std::shared_ptr<Shader> shader) {
	if (electricBoxLight) {
		// electric box light
		shader->setUniform("pointLights[0].color", glm::vec3(0.5f, 0.125f, 0.0f));
		shader->setUniform("pointLights[0].position", glm::vec3(-3.95f, 1.2f, -1.75f));
		shader->setUniform("pointLights[0].attenuation", glm::vec3(1.0f, 0.7f, 1.8f));
	}
	else {
		// elevator light
		shader->setUniform("pointLights[0].color", glm::vec3(0.7f, 0.9f, 1.0f));
		shader->setUniform("pointLights[0].position", glm::vec3(-6.0f, 1.8f, 0.0f));
		shader->setUniform("pointLights[0].attenuation", glm::vec3(1.0f, 0.7f, 1.8f));
	}
	shader->setUniform("amountOfPointLights", 1);
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
	physx::PxExtendedVec3 pos = controller->getFootPosition();
	glm::vec3 position = glm::vec3(pos.x, pos.y + CHARACTER_EYE_HEIGHT, pos.z);
	float cam_offset = sin(bobbingTime) / 12; //higher value makes the bobbing effect weaker
	return glm::lookAt(position + right * cam_offset, (position + front) + right * cam_offset, up);
}

glm::mat4 Camera::getProjectionMatrix() {
	return projectionMatrix;
}

void Camera::processKeyEvent(unsigned char keys, bool isRunning, float deltaTime) {
	float velocity_multiplier = (isRunning ? 2.0f : 1.0f) * deltaTime * ((keys & S || keys & A || keys & D) ? 0.7f : 1.0f);

	if (keys & W)
		displacement += physx::PxVec3(front.x, 0, front.z);
	if (keys & S)
		displacement += physx::PxVec3(-front.x, 0, -front.z);
	if (keys & A)
		displacement += physx::PxVec3(-right.x, 0, -right.z);
	if (keys & D)
		displacement += physx::PxVec3(right.x, 0, right.z);

	displacement = displacement.getNormalized() * MOVEMENT_SPEED * velocity_multiplier;

	bool before = sin(bobbingTime) < 0;
	bobbingTime = fmod(bobbingTime + 4.5f /*higher value makes bobbing effect faster*/ * velocity_multiplier, 2 * M_PI);
	playstepsound = sin(bobbingTime) < 0 != before;
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
	displacement.y = -9.81f * deltaTime;

	// movement
	controller->move(displacement, 0.01f, deltaTime, filters);

	// new listener position and orientation
	physx::PxExtendedVec3 position = controller->getFootPosition();
	position.y += CHARACTER_EYE_HEIGHT;
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	ALfloat ori[6] = { front.x, front.y, front.z, 0, 1, 0 };
	alListenerfv(AL_ORIENTATION, ori);

	if (playstepsound) {
		if (water_concrete)
			alSourcei(audioSources[!usesource], AL_BUFFER, concreteBuffers[distr(generator)]);
		else
			alSourcei(audioSources[!usesource], AL_BUFFER, waterBuffers[distr(generator)]);
		alSourcePlay(audioSources[usesource]);
		usesource = !usesource;
		playstepsound = false;
	}


	// reset vector
	displacement.x = 0.0f;
	displacement.z = 0.0f;
}

bool Camera::raycast(physx::PxRaycastBuffer &hit) {
	extern physx::PxScene *pxScene;
	physx::PxExtendedVec3 pos = controller->getFootPosition();
	physx::PxVec3 origin = physx::PxVec3(pos.x, pos.y + CHARACTER_EYE_HEIGHT, pos.z);
	physx::PxVec3 destination = physx::PxVec3(front.x, front.y, front.z);
	physx::PxReal range = physx::PxReal(1.5f);
	const physx::PxHitFlags outputFlags = physx::PxHitFlag::ePOSITION;
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	filterData.data.word1 = INTERACTABLE;
	return pxScene->raycast(origin, destination, range, hit, outputFlags, filterData);
}

void Camera::turnElectricBoxOff() {
	electricBoxLight = false;
	alSourcePlay(neonSource);
}

void Camera::turnSpotlightOn() {
	cameraLight.isTurnedOn = true;
}

void Camera::turnSpotlightOff() {
	cameraLight.isTurnedOn = false;
}

void Camera::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	if (water_concrete = hit.shape->getSimulationFilterData().word3 != WATER) {
		alSourcei(audioSources[usesource], AL_BUFFER, concreteBuffers[distr(generator)]);
		alSourcei(audioSources[!usesource], AL_BUFFER, concreteBuffers[distr(generator)]);
	}
	else {
		alSourcei(audioSources[usesource], AL_BUFFER, waterBuffers[distr(generator)]);
		alSourcei(audioSources[!usesource], AL_BUFFER, waterBuffers[distr(generator)]);
	}
}

void Camera::updateBrightnessModifier(float deltaT) {
	// lamp starts flickering when electric box is repaired
	if (!electricBoxLight) {
		// if brightnessModifier < 0 lamp is off
		if (brightnessModifier < 0) {
			// if brightnessModifier + deltaT >= 0 calculate new value
			if (brightnessModifier + deltaT >= 0) {
				std::random_device rd;
				std::mt19937 eng(rd());
				std::uniform_real_distribution<> distr(-0.1f, 0.9f);

				brightnessModifier = distr(eng);
			}
			else
				brightnessModifier += deltaT;
		}
		// if brightnessModifier >= 0 lamp is on
		else {
			// if brightnessModifier - deltaT < 0 calculate new value
			if (brightnessModifier - deltaT < 0) {
				std::random_device rd;
				std::mt19937 eng(rd());
				std::uniform_real_distribution<> distr(-0.1f, 0.3f);

				brightnessModifier = distr(eng);
			}
			else
				brightnessModifier -= deltaT;
		}
	}
}
