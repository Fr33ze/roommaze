#pragma once

#include <iostream>
#include <sstream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "Shader.h"
#include "Camera.h"

class Particles {

protected:

	// shader used for rendering particles
	std::shared_ptr<Shader> shader;

	struct ParticleObject {
		glm::vec3 position, velocity;
		float gravity;
		float remainingLifeTime;
		float rotation;
		float size;

		ParticleObject(glm::vec3 position, glm::vec3 velocity, float gravity, float remainingLifeTime, float rotation, float size)
			: position(position), velocity(velocity), gravity(gravity), remainingLifeTime(remainingLifeTime), rotation(rotation), size(size) {
		};

		~ParticleObject() {}
	};

	std::vector<ParticleObject> particles;

	glm::vec3 origin, velocity;
	float gravity;
	float lifeLength;
	float particleSize;

	GLuint vao;
	GLuint vboVertices;

	void createNewParticle();

public:
	
	/**
	 * CONSTRUCTOR
	 *
	 * Blablabla, da kommt dann noch was hin. ;D
	 *
	 * @param orogin: blablabla
	 */
	Particles(glm::vec3 origin, glm::vec3 velocity, float gravity, float lifeLength, float particleSize);
	
	Particles();

	~Particles();

	void draw(float deltaTime);

};
