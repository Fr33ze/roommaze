#include "GUIComponent.h"

GUIComponent::GUIComponent(std::string texturePath, glm::mat4 modelMatrix)
	: modelMatrix(modelMatrix) {

	initTexture(texturePath);

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

GUIComponent::GUIComponent() {

}

GUIComponent::~GUIComponent() {

}

void GUIComponent::initTexture(std::string texturePath) {
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// load texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		switch (nrChannels) {
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		std::cout << "Failed to load texture from: " + texturePath << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	stbi_image_free(data);
}

void GUIComponent::draw(std::shared_ptr<Shader> shader) {
	shader->use();

	shader->setUniform("modelMatrix", modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	shader->setUniform("textureUnit", 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // enable rendering semi-transparent materials
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how blendig is accomplished
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	shader->unuse();
}
