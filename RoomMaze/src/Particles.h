#pragma once

#include <iostream>
#include <sstream>
#include <random>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "Shader.h"
#include "Camera.h"

static const int MAX_PARTICLES = 10000;

class Particles {

protected:

	// shader used for rendering particles
	std::shared_ptr<Shader> shader;

	glm::vec3 origin, speed;
	float size, weight, lifeLength;

	struct ParticleObject {
		glm::vec3 position, speed;
		float size, angle, weight, remainingLifeTime, cameraDistance;

		bool operator<(const ParticleObject& that) const {
			// sort in reverse order --> far particles drawn first
			return this->cameraDistance > that.cameraDistance;
		}
	};
	ParticleObject particles[MAX_PARTICLES];
	int lastUsedParticle;
	int particleCounter;

	float *positions = new float[4 * MAX_PARTICLES];

	GLuint vao;
	GLuint vboVertices;
	GLuint vboPositionsAndScaling;

	
	void createNewParticles(int amount);
	void updateParticles(float deltaTime);
	int getUnusedParticle();
	void sortParticles();

public:
	
	Particles(glm::vec3 origin, glm::vec3 speed, float size, float weight, float lifeLength);
	
	Particles();

	~Particles();

	void draw(float deltaTime);

};
