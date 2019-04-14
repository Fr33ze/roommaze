#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

// stores all directions
enum Key {
	W, // forward
	S, // backward
	A, // left
	D // right
};

// default camera values
static const float MOVEMENT_SPEED = 1.0f;
static const float MOUSE_SENSITIVITY = 0.1f;

class Camera {

protected:
	
	// camera attributes
	glm::vec3 position;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// the light of the camera is a spot light
	struct CameraLight {
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

	/**
	 * Sets the parameters of the camera's spot light.
	 *
	 * @param intensity: intensity (= color) of the light
	 * @param innerAngle: inner angle of the light cone
	 * @param outerAngle: outer angle of the light cone
	 * @param attenuation: vec3 of floats that stores the attenuation of the light (constant, linear, quadratic)
	 */
	void setSpotLightParameters(glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation);

	/**
	 * Sets camera's parameters as uniforms in the specified shader.
	 *
	 * @param shader: shader used for rendering this camera
	 */
	void setUniforms(std::shared_ptr<Shader> shader);

	~Camera();

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

};
