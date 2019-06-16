#include "Particles.h"

Particles::Particles(glm::vec3 position, float size) {
	size = size / 2.0f;

	shader = std::make_shared<Shader>("assets/shaders/particles.vert", "assets/shaders/particles.frag");

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	float vertices[] = { position.x - size, position.y + size,
		position.x - size, position.y - size,
		position.x + size, position.y + size,
		position.x + size, position.y - size };
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

Particles::Particles() {

}

Particles::~Particles() {

}

void Particles::draw(float deltaTime) {
	shader->use();
	
	extern Camera *camera;
	shader->setUniform("viewMatrix", camera->getViewMatrix());
	shader->setUniform("projectionMatrix", camera->getProjectionMatrix());
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	shader->unuse();
}
