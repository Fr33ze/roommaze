#include "GUIText.h"

GUIText::GUIText(std::string fontPath, std::string text, glm::vec2 position, float scale, float alpha, int windowWidth, int windowHeight)
	: text(text), position(position), scale(scale), alpha(alpha), projectionMatrix(glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight)) {
	
	initFont(fontPath);

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// VBO for UV coords
	glGenBuffers(1, &vboUVCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboUVCoords);
	float UVCoords[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), UVCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

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
	if (FT_Init_FreeType(&library)) {
		std::cout << "Failed to init FreeType library." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	FT_Face face;
	if (FT_New_Face(library, fontPath.c_str(), 0, &face)) {
		std::cout << "Failed to load the font of the gui." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	// set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 100);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	for (unsigned char c = 0; c < 128; c++) {

		// load glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "Failed to load glyph " + std::to_string(c) + " of the gui's font." << std::endl;
			continue;
		}

		GLuint textureHandle;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);

		// load texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store character for later use
		Character character = { textureHandle, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x };
		characters.insert(std::pair<GLchar, Character>(c, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// destroy FreeType stuff once we are finished
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void GUIText::draw(std::shared_ptr<Shader> shader) {
	if (!text.empty()) {
		shader->use();

		shader->setUniform("projectionMatrix", projectionMatrix);
		shader->setUniform("alpha", alpha);

		glActiveTexture(GL_TEXTURE0);
		shader->setUniform("textureUnit", 0);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND); // enable rendering semi-transparent materials
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how blendig is accomplished

		glBindVertexArray(vao);

		float pointer = position.x;

		// iterate through all characters
		std::string::const_iterator interator;
		for (interator = text.begin(); interator != text.end(); interator++) {
			Character character = characters[*interator];

			float xPos = pointer + character.bearing.x * scale;
			float yPos = position.y - (character.size.y - character.bearing.y) * scale;

			float width = character.size.x * scale;
			float height = character.size.y * scale;

			// set vertices
			float vertices[] = { xPos, yPos + height, xPos, yPos, xPos + width, yPos + height, xPos + width, yPos };
			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // glBufferSubData instead of glBufferData
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindTexture(GL_TEXTURE_2D, character.textureHandle);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			// advance cursors for next glyph (advance is number of 1/64 pixels)
			pointer += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		shader->unuse();
	}
}

void GUIText::updateText(std::string text) {
	GUIText::text = text;
}
