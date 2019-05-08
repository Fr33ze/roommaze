#pragma once

#include <iostream>
#include <sstream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include "stb_image.h"

#include "Shader.h"

class GUIComponent {

protected:

	GLuint textureHandle;
	glm::mat4 modelMatrix;
	GLuint vao;
	GLuint vboVertices;

	void initTexture(std::string texturePath);

public:

	/**
	 * CONSTRUCTOR
	 * Creates a gui component with the given parameters.
	 *
	 * @param texturePath: path to the texture of the gui component
	 * @param modelMatrix: the model matrix of the gui component
	 */
	GUIComponent(std::string texturePath, glm::mat4 modelMatrix);
	
	GUIComponent();

	~GUIComponent();

	void draw(std::shared_ptr<Shader> shader);

};
