#include "Particles.h"

Particles::Particles(glm::vec3 origin, glm::vec3 velocity, float gravityEffect, float lifeLength, float scale)
	: origin(origin), velocity(velocity), gravityEffect(gravityEffect), lifeLength(lifeLength), scale(scale) {

	shader = std::make_shared<Shader>("assets/shaders/particles.vert", "assets/shaders/particles.frag");

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	float vertices[] = {
		-0.5,  0.5, 0.0f,
		-0.5, -0.5, 0.0f,
		 0.5,  0.5, 0.0f,
		 0.5, -0.5, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// VBO for positions and scaling
	glGenBuffers(1, &vboPositionsAndScaling);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionsAndScaling);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	createNewParticles(10);
}

Particles::Particles() {

}

Particles::~Particles() {

}

void Particles::createNewParticles(int amount) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(81, 120);

	for (int i = 0; i < amount; i++) {
		ParticleObject particle = ParticleObject(origin, velocity * glm::vec3(distr(eng) / 100.0f, distr(eng) / 100.0f, distr(eng) / 100.0f), gravityEffect, lifeLength, 0.0f, scale);
		particles.push_back(particle);
		positions.push_back(glm::vec4(origin, scale));
	}
}

void Particles::draw(float deltaTime) {
	this->deltaTime = deltaTime;

	updateParticles();

	// -----------------
	// RENDER PARTICLES
	// -----------------

	shader->use();
	
	extern Camera *camera;
	glm::mat4 viewMatrix = camera->getViewMatrix();
	shader->setUniform("viewProjectionMatrix", camera->getProjectionMatrix() * viewMatrix);
	shader->setUniform("cameraRightWorldspace", glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
	shader->setUniform("cameraUpWorldspace", glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionsAndScaling);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec4), positions.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, positions.size());

	glBindVertexArray(0);

	shader->unuse();
}

void Particles::updateParticles() {
	// clear positions vector
	positions.clear();
	for (int i = 0; i < particles.size(); i++) {
		// update remaining life time of the particle
		particles.at(i).remainingLifeTime -= deltaTime;
		// delete dead particle from particles vector
		if (particles.at(i).remainingLifeTime <= 0.0f) {
			particles.erase(particles.begin() + i);
		}
		// alive particle
		else {
			// update particle's velocity & position
			particles.at(i).velocity += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.5f;
			particles.at(i).position += particles.at(i).velocity * deltaTime;
			// inster particle into positions vector
			positions.push_back(glm::vec4(particles.at(i).position, particles.at(i).scale));
		}
	}

	// generate 10 new particles each millisecond
	int amountOfNewParticles = (int)(deltaTime * 10000.0f);
	// limiter to 16 ms (60 FPS)
	if (amountOfNewParticles > (int)(0.016f * 10000.0f)) {
		amountOfNewParticles = (int)(0.016f * 10000.0f);
	}
	createNewParticles(amountOfNewParticles);
}
