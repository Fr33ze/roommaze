#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	// eurler angles
	float yaw;
	float pitch;

	void updateCameraVectors();

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

	glm::vec3 getPosition();

	glm::mat4 getViewMatrix();

	glm::mat4 getProjectionMatrix();

};
