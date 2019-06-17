#include "Particles.h"

Particles::Particles(glm::vec3 origin, glm::vec3 speed, float size, float weight, float lifeLength)
	: origin(origin), speed(speed), size(size), weight(weight), lifeLength(lifeLength), lastUsedParticle(0) {

	shader = std::make_shared<Shader>("assets/shaders/particles.vert", "assets/shaders/particles.frag");

	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].remainingLifeTime = 0.0f;
		particles[i].cameraDistance = 0.0f;
	}

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

	// VBO for positions and scaling
	glGenBuffers(1, &vboPositionsAndScaling);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionsAndScaling);
	glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES * sizeof(float), nullptr, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
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
	std::uniform_int_distribution<> distr(91, 110);

	for (int i = 0; i < amount; i++) {
		int index = getUnusedParticle();
		ParticleObject &particle = particles[index];

		particle.position = origin;
		particle.speed = speed * glm::vec3(distr(eng) / 100.0f, distr(eng) / 100.0f, distr(eng) / 100.0f);
		particle.size = size * distr(eng) / 100.0f;
		particle.remainingLifeTime = lifeLength;
	}
}

void Particles::updateParticles(float deltaTime) {
	extern Camera *camera;

	int particleCounter = 0;
	for (int i = 0; i < MAX_PARTICLES; i++) {
		ParticleObject &particle = particles[i];
		if (particle.remainingLifeTime > 0.0f) {
			// update remaining life time of the particle
			particle.remainingLifeTime -= deltaTime;
			if (particle.remainingLifeTime > 0.0f) {
				// particle simulation
				particle.speed += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.5f;
				particle.position += particle.speed * deltaTime;
				particle.cameraDistance = glm::length(particle.position - camera->getPosition());
				
				positions[4 * particleCounter + 0] = particle.position.x;
				positions[4 * particleCounter + 1] = particle.position.y;
				positions[4 * particleCounter + 2] = particle.position.z;
				positions[4 * particleCounter + 3] = particle.size;

			}
		} else {
			particle.cameraDistance = 0.0f;
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

int Particles::getUnusedParticle() {
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
		if (particles[i].remainingLifeTime <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particles[i].remainingLifeTime <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // all particles are taken, override the first one
}

void Particles::sortParticles() {
	std::sort(&particles[0], &particles[MAX_PARTICLES]);
}

void Particles::draw(float deltaTime) {
	updateParticles(deltaTime);

	// -----------------
	// RENDER PARTICLES
	// -----------------

	shader->use();
	
	

	

	shader->unuse();
}