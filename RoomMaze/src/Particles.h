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

	GLuint vao;
	GLuint vboVertices;

public:
	
	Particles(glm::vec3 position, float size);
	
	Particles();

	~Particles();

	void draw(float deltaTime);

};
