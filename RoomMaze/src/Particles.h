#pragma once

#include <iostream>
#include <sstream>
#include <random>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"

static const int MAX_AMOUNT_OF_PARTICLES = 10000;

class Particles {

protected:

	// shader used for rendering particles
	std::shared_ptr<Shader> shader;

	// defines whether particle spawning is enabled or not
	// -1.0f == enabled with no timer
	// 0.0f == disabled
	// >0.0f == enabled with timer
	float enabledForSeconds;

	float particleSpawningRate;
	float secondCounter = 0.0f;

	glm::vec3 origin, speed;
	float size, weight, lifeLength, randomizeSpeedFactor;

	struct ParticleObject {
		glm::vec3 position, speed;
		float size, weight, remainingLifeTime;
	};
	std::vector<ParticleObject> particles;
	std::vector<float> positions;

	GLuint vao;
	GLuint vboVertices;
	GLuint vboPositionsAndScaling;

	GLuint textureHandle;

	void initTexture(std::string texturePath);
	void createNewParticles(int amount);
	void updateParticles(float deltaTime);

public:
	
	/**
	 * CONSTRUCTOR
	 * Creates particles around the specified origin.
	 *
	 * @param particleSpawningRate: defines after how many seconds a new particle should spawn
	 * @param origin: position where all particles spawn
	 * @param speed: defines the particle's direction (length of the vector is the speed)
	 * @param size: defines the size in meters of a single particle
	 * @param weight: defines the weight of a single particle (how it is affected by gravity)
	 * @param lifeLength: defines after how many seconds a particle disappears
	 * @param randomizeSpeedFactor: defines how much the randomizer is allowed to change the speed vector (the lower the value, the higher the randomization)
	 * @param texturePath: path to the texture for the particles
	 * @param isEnabled: defines whether particle spawning is enabled or not
	 */
	Particles(float particleSpawningRate, glm::vec3 origin, glm::vec3 speed, float size, float weight, float lifeLength, float randomizeSpeedFactor, std::string texturePath, bool isEnabled);
	
	Particles();

	~Particles();

	void setOrigin(glm::vec3 position);

	void draw(float deltaTime);

	void enable();

	void disable();

	void enableFor(float seconds);

};
