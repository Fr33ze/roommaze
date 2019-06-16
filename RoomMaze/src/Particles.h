#pragma once

#include <iostream>
#include <sstream>
#include <random>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "Shader.h"
#include "Camera.h"

class Particles {

protected:

	// shader used for rendering particles
	std::shared_ptr<Shader> shader;

	float deltaTime;

	struct ParticleObject {
		glm::vec3 position, velocity;
		float gravityEffect, remainingLifeTime, rotation, scale;

		ParticleObject(glm::vec3 position, glm::vec3 velocity, float gravityEffect, float remainingLifeTime, float rotation, float scale)
			: position(position), velocity(velocity), gravityEffect(gravityEffect), remainingLifeTime(remainingLifeTime), rotation(rotation), scale(scale) {
		};

		~ParticleObject() {}
	};
	std::vector<ParticleObject> particles;
	std::vector<glm::vec4> positions;

	glm::vec3 origin, velocity;
	float gravityEffect, lifeLength, scale;

	GLuint vao;
	GLuint vboVertices;
	GLuint vboPositionsAndScaling;

	void createNewParticles(int amount);
	void updateParticles();

public:
	
	Particles(glm::vec3 origin, glm::vec3 velocity, float gravityEffect, float lifeLength, float scale);
	
	Particles();

	~Particles();

	void draw(float deltaTime);

};
