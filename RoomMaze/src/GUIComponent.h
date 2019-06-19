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

	glm::mat4 projectionMatrix;
	GLuint vao;
	GLuint vboVertices;
	GLuint vboUVCoords;

	void initTexture(std::string texturePath);

public:

	/**
	 * CONSTRUCTOR
	 * Creates a gui component with the given parameters.
	 *
	 * @param texturePath: path to the texture of the gui component
	 * @param position: defines where to place the upper left corner of the component
	 * @param size: defines the size of the component as a percentage of the screen width (!) it occupies (must be between 0.0f & 1.0f)
	 * @param windowWidth: the width of the window
	 * @param windowHeight: the height of the window
	 * @param scaleToScreenSize: defines whether the texture should be scaled up so that it covers the whole screen
	 */
	GUIComponent(std::string texturePath, glm::vec2 position, float size, int windowWidth, int windowHeight, bool scaleToScreenSize);
	
	GUIComponent();

	~GUIComponent();

	void draw(std::shared_ptr<Shader> shader);

};
