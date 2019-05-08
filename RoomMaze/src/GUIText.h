#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include "stb_image.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

class GUIText {

protected:

	struct Character {
		GLuint textureHandle;
		glm::ivec2 size;
		glm::ivec2 bearing; // offset from baseline to left-top of glyph
		GLuint advance; // horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> characters;
	
	std::string text;
	glm::mat4 modelMatrix;
	GLuint vao;
	GLuint vboVertices;

	void initFont(std::string fontPath);

public:
	
	/**
	 * CONSTRUCTOR
	 * Creates a gui text with the given parameters.
	 *
	 * @param fontPath: path to the font
	 * @param text: text to be rendered
	 * @param modelMatrix: the model matrix of the text
	 */
	GUIText(std::string fontPath, std::string text, glm::mat4 modelMatrix);
	
	GUIText();

	~GUIText();

	void draw(std::shared_ptr<Shader> shader);

	void updateText(std::string text);

};
