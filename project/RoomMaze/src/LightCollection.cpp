#include "LightCollection.h"

LightCollection::LightCollection() {

}

LightCollection::~LightCollection() {

}

void LightCollection::addDirectionalLight(glm::vec3 color, glm::vec3 direction) {
	directionalLights.push_back({ color, direction });
}

void LightCollection::addPointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation) {
	pointLights.push_back({ color, position, attenuation });
}

void LightCollection::addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation) {
	spotLights.push_back({ position, direction, intensity, innerAngle, outerAngle, attenuation });
}

void LightCollection::setUniforms(std::shared_ptr<Shader> shader) {
	for (unsigned int i = 0; i < directionalLights.size(); i++) {
		shader->setUniform("directionalLights[" + std::to_string(i) + "].color", directionalLights.at(i).color);
		shader->setUniform("directionalLights[" + std::to_string(i) + "].direction", directionalLights.at(i).direction);
	}
	shader->setUniform("amountOfDirectionalLights", directionalLights.size());

	for (unsigned int i = 0; i < pointLights.size(); i++) {
		shader->setUniform("pointLights[" + std::to_string(i) + "].color", pointLights.at(i).color);
		shader->setUniform("pointLights[" + std::to_string(i) + "].position", pointLights.at(i).position);
		shader->setUniform("pointLights[" + std::to_string(i) + "].attenuation", pointLights.at(i).attenuation);
	}
	shader->setUniform("amountOfPointLights", pointLights.size());

	for (unsigned int i = 0; i < spotLights.size(); i++) {
		shader->setUniform("spotLights[" + std::to_string(i) + "].position", spotLights.at(i).position);
		shader->setUniform("spotLights[" + std::to_string(i) + "].direction", spotLights.at(i).direction);
		shader->setUniform("spotLights[" + std::to_string(i) + "].intensity", spotLights.at(i).intensity);
		shader->setUniform("spotLights[" + std::to_string(i) + "].innerCutOff", glm::cos(glm::radians(spotLights.at(i).innerAngle))); // cos()-calculation outside shader (saves valuable time)
		shader->setUniform("spotLights[" + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(spotLights.at(i).outerAngle)));
		shader->setUniform("spotLights[" + std::to_string(i) + "].attenuation", spotLights.at(i).attenuation);
	}
	shader->setUniform("amountOfSpotLights", spotLights.size());
}
