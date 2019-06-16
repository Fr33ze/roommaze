#include "Particles.h"

Particles::Particles(glm::vec3 origin, glm::vec3 velocity, float gravity, float lifeLength, float particleSize)
	: origin(origin), velocity(velocity), gravity(gravity), lifeLength(lifeLength), particleSize(particleSize) {

	shader = std::make_shared<Shader>("assets/shaders/particles.vert", "assets/shaders/particles.frag");

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	createNewParticle();
}

Particles::Particles() {

}

Particles::~Particles() {

}

void Particles::createNewParticle() {
	particles.push_back(ParticleObject(origin, velocity, gravity, lifeLength, 0.0f, particleSize));
}

void Particles::draw(float deltaTime) {
	shader->use();
	
	extern Camera *camera;
	shader->setUniform("viewMatrix", camera->getViewMatrix());
	shader->setUniform("projectionMatrix", camera->getProjectionMatrix());
	
	glBindVertexArray(vao);

	glm::vec3 tempPosition;
	float tempSize;
	for (unsigned int i = 0; i < particles.size(); i++) {
		// TODO: update vertice according to camera position

		tempPosition = particles.at(i).position;
		tempSize = particles.at(i).size / 2.0f;
		float vertices[] = { tempPosition.x + tempSize, tempPosition.y + tempSize, tempPosition.z,
			tempPosition.x + tempSize, tempPosition.y - tempSize, tempPosition.z,
			tempPosition.x - tempSize, tempPosition.y + tempSize, tempPosition.z,
			tempPosition.x - tempSize, tempPosition.y - tempSize, tempPosition.z};
		glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // glBufferSubData instead of glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindVertexArray(0);

	shader->unuse();
}
