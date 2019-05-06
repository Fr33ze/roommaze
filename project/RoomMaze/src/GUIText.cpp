#include "GUIText.h"

GUIText::GUIText(std::string fontPath, std::string text, glm::mat4 modelMatrix)
	: text(text), modelMatrix(modelMatrix) {
	
	initFont(fontPath);

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	float vertices[] = { -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0 };
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GUIText::GUIText() {

}

GUIText::~GUIText() {

}

void GUIText::initFont(std::string fontPath) {
	FT_Library library;
	FT_Face face;

	if (FT_Init_FreeType(&library)) {
		std::cout << "Failed to init FreeType." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	if (FT_New_Face(library, "assets/gui/Letter Gothic Regular.ttf", 0, &face)) {
		std::cout << "Failed to load the font of the gui." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	FT_Set_Pixel_Sizes(face, 0, 50);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set.
	for (GLubyte i = 0; i < 128; i++) {
		// load character glyph 
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			std::cout << "Failed to load glyph#" + std::to_string(i) + " of the gui's font." << std::endl;
			continue;
		}

		// generate texture
		GLuint textureHandle;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store character for later use
		Character character = { textureHandle, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x };
		characters.insert(std::pair<GLchar, Character>(i, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// destroy FreeType stuff once we are finished
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void GUIText::draw(std::shared_ptr<Shader> shader) {
	shader->use();

	glActiveTexture(GL_TEXTURE0);
	shader->setUniform("textureUnit", 0);

	glEnable(GL_BLEND); // enable rendering semi-transparent materials
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how blendig is accomplished

	glBindVertexArray(vao);

	glm::mat4 tempModelMatrix = modelMatrix;

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		shader->setUniform("modelMatrix", tempModelMatrix);

		Character ch = characters[*c];
		glBindTexture(GL_TEXTURE_2D, ch.textureHandle);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		tempModelMatrix = glm::translate(tempModelMatrix, glm::vec3(ch.advance / 700.0f, 0.0f, 0.0f));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	shader->unuse();
}

void GUIText::updateText(std::string text) {
	GUIText::text = text;
}
