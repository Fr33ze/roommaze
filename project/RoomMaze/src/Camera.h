#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Key {
	W, // forward
	S, // backward
	A, // left
	D // right
};

// default camera values
static const float MOVEMENT_SPEED = 0.5f;
static const float MOUSE_SENSITIVITY = 0.05f;

class Camera {

protected:
	
	// camera attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// eurler angles
	float yaw;
	float pitch;

	// camera options
	float movementSpeed;
	float mouseSensitivity;

	void updateCameraVectors();

public:
	
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
	
	~Camera();

	void processKeyEvent(Key key, float deltaTime);

	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	glm::vec3 getPosition();

	glm::mat4 getViewMatrix();

};
