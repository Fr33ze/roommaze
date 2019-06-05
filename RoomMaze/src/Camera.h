#pragma once

#define _USE_MATH_DEFINES

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

#include "Shader.h"
#include "CharacterCallback.h"

// stores all directions
enum Key {
	W, // forward
	S, // backward
	A, // left
	D // right
};

// scene query groups for shapes
enum ActiveGroup
{
	COLLISION = (1 << 0),
	INTERACTABLE = (1 << 1)
};

// default camera values
static const float MOVEMENT_SPEED = 3.0f;
static const float MOUSE_SENSITIVITY = 0.1f;

// character constants
static const float CHARACTER_HEIGHT = 1.7f;
static const float CHARACTER_RADIUS = 0.2f;
static const float CHARACTER_EYE_HEIGHT = 1.6f;

class Camera {

protected:

	// displacement (movement) vector
	physx::PxVec3 displacement;

	// physx controller stuff
	physx::PxControllerManager* cManager;
	physx::PxController* controller;
	CharacterCallback* cctCallbacks;
	physx::PxControllerFilters filters;

	// camera attributes
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// time to calculate bobbing effect (ranges from 0-2PI)
	float bobbingTime;

	// the light of the camera is a spot light
	struct CameraLight {
		float brightness;
		bool isTurnedOn;
		glm::vec3 intensity;
		float innerAngle;
		float outerAngle;
		glm::vec3 attenuation;
	};
	CameraLight cameraLight;

	// eurler angles
	float yaw;
	float pitch;

	void updateCameraVectors();

	glm::mat4 getViewMatrix();

public:
	
	/**
	 * CONSTRUCTOR
	 * Creates a FPS camera.
	 *
	 * @param position: position of the camera
	 * @param fieldOfView: the field of view used for calculating the projection matrix
	 * @param aspectRatio: the ratio of window width to window height (width / height)
	 */
	Camera(glm::vec3 position, float fieldOfView, float aspectRatio);

	Camera();

	~Camera();

	/**
	 * Sets the parameters of the camera's spot light.
	 *
	 * @param brightness: brightness of the whole scene
	 * @param intensity: intensity (= color) of the light
	 * @param innerAngle: inner angle of the light cone
	 * @param outerAngle: outer angle of the light cone
	 * @param attenuation: vec3 of floats that stores the attenuation of the light (constant, linear, quadratic)
	 */
	void setSpotLightParameters(float brightness, glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation);

	/**
	 * Sets camera's parameters as uniforms in the given shader.
	 *
	 * @param shader: shader used for rendering
	 */
	void setUniforms(std::shared_ptr<Shader> shader);

	/**
	 * Updates the camera concerning the pressed key.
	 * 
	 * @param key: the pressed key
	 * @param isRunning: true if velocity should be higher as usual (running character)
	 * @param deltaTime: time between current frame and last frame
	 */
	void processKeyEvent(Key key, bool isRunning, float deltaTime);

	/**
	 * Updates the camera concerning the mouse movement.
	 *
	 * @param xOffset: the distance of the cursor between current frame and last frame concerning the x-axis
	 * @param yOffset: the distance of the cursor between current frame and last frame concerning the y-axis
	 * @param constraintPitch: when pitch is out of bounds, screen doesn't get flipped
	 */
	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	/**
	 * Moves the character (and camera) according to gravity and input
	 *
	 * @param deltaTime: time between current frame and last frame
	 */
	void move(float deltaTime);

	bool raycast(physx::PxRaycastBuffer &hit);

	void turnSpotlightOn();

	void turnSpotlightOff();

};
