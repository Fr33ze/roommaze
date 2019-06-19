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
		GLuint textureHandle; // texture handle of the glyph texture
		glm::ivec2 size; // size of glyph
		glm::ivec2 bearing; // offset from baseline to left/top of glyph
		GLuint advance; // horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> characters;
	
	std::string text;
	glm::vec2 position;
	float scale;

	// used when the centered text overflows max_width
	float overflowscale = 1.f;
	float max_width = 0.7f;

	int windowWidth;

	glm::mat4 projectionMatrix;
	float alpha;
	GLuint vao;
	GLuint vboVertices;
	GLuint vboUVCoords;

	void initFont(std::string fontPath);

public:
	
	/**
	 * CONSTRUCTOR
	 * Creates a gui text with the given parameters.
	 *
	 * @param fontPath: path to the font
	 * @param text: text to be rendered
	 * @param position: defines where to place the lower left corner of the text
	 * @param scale: scale factor of the text
	 * @param alpha: defines the opacity of the text
	 * @param windowWidth: the width of the window
	 * @param windowHeight: the height of the window
	 */
	GUIText(std::string fontPath, std::string text, glm::vec2 position, float scale, float alpha, int windowWidth, int windowHeight);
	
	GUIText();

	~GUIText();

	void draw(std::shared_ptr<Shader> shader);

	void updateText(std::string text);

	//updates text and centers it horizontally (y coordinate still used)
	void updateTextCenter(std::string text);
};
