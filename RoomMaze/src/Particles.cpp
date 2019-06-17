#include "Particles.h"

Particles::Particles(float particleSpawningRate, glm::vec3 origin, glm::vec3 speed, float size, float weight, float lifeLength, std::string texturePath, bool isEnabled)
	: particleSpawningRate(particleSpawningRate), origin(origin), speed(speed), size(size), weight(weight), lifeLength(lifeLength), enabledForSeconds(isEnabled ? -1.0f : 0.0f) {

	shader = std::make_shared<Shader>("assets/shaders/particles.vert", "assets/shaders/particles.frag");

	initTexture(texturePath);

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
	glVertexAttribDivisor(0, 0);

	// VBO for positions and scaling
	glGenBuffers(1, &vboPositionsAndScaling);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionsAndScaling);
	glBufferData(GL_ARRAY_BUFFER, MAX_AMOUNT_OF_PARTICLES, nullptr, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glVertexAttribDivisor(1, 1);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	createNewParticles(1);
}

Particles::Particles() {

}

Particles::~Particles() {

}

void Particles::initTexture(std::string texturePath) {
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// load texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		switch (nrChannels) {
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;
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
	}
	else {
		std::cout << "Failed to load texture from: " + texturePath << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	stbi_image_free(data);
}

void Particles::createNewParticles(int amount) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 1000);

	for (int i = 0; i < amount && particles.size() < MAX_AMOUNT_OF_PARTICLES; i++) {
		ParticleObject particle;
		particle.position = origin;
		particle.speed = speed * glm::vec3(distr(eng) / 100.0f, distr(eng) / 100.0f, distr(eng) / 100.0f);
		particle.size = size;
		particle.remainingLifeTime = lifeLength;
		particles.push_back(particle);
	}
}

void Particles::updateParticles(float deltaTime) {
	// clear positions vector
	positions.clear();
	for (int i = 0; i < particles.size(); i++) {
		ParticleObject &particle = particles.at(i);
		// update remaining life time of the particle
		particle.remainingLifeTime -= deltaTime;
		// delete dead particle from particles vector
		if (particle.remainingLifeTime <= 0.0f) {
			particles.erase(particles.begin() + i);
		}
		// alive particle
		else {
			// update particle's speed & position
			particle.speed += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.5f;
			particle.position += particle.speed * deltaTime;
			// inster particle into positions vector
			positions.push_back(particle.position.x);
			positions.push_back(particle.position.y);
			positions.push_back(particle.position.z);
			positions.push_back(particle.size);
		}
	}

	secondCounter += deltaTime;
	if (secondCounter >= particleSpawningRate) {
		secondCounter = 0.0f;
		createNewParticles(1);
	}
}

void Particles::draw(float deltaTime) {
	// check if particle spawning is enabled
	if (enabledForSeconds == -1.0f || enabledForSeconds > 0.0f) {
		if (enabledForSeconds > 0.0f) {
			enabledForSeconds -= deltaTime;
		}

		updateParticles(deltaTime);

		// update buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionsAndScaling);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());

		// render
		shader->use();

		extern Camera *camera;
		glm::mat4 viewMatrix = camera->getViewMatrix();
		shader->setUniform("cameraRightWorldspace", glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
		shader->setUniform("cameraUpWorldspace", glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
		shader->setUniform("viewProjectionMatrix", camera->getProjectionMatrix() * camera->getViewMatrix());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		shader->setUniform("textureUnit", 0);

		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND); // enable rendering semi-transparent materials
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // set how blendig is accomplished

		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, positions.size() / 4);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		shader->unuse();
	}
}

void Particles::enable() {
	enabledForSeconds = -1.0f;
}

void Particles::disable() {
	enabledForSeconds = 0.0f;
}

void Particles::enableFor(float seconds) {
	enabledForSeconds = seconds;
}
