#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float alpha, std::string pathAmbientTextureMap, std::string pathDiffuseTextureMap, std::string pathSpecularTextureMap, std::string pathShininessTextureMap, std::string pathAlphaTextureMap)
	: shader(shader), ambientColor(ambientColor), diffuseColor(diffuseColor), specularColor(specularColor), shininess(shininess), alpha(alpha) {

	if (!pathAmbientTextureMap.empty()) {
		hasAmbientTextureMap = true;
		initTexture(pathAmbientTextureMap, ambientTextureMapHandle);
	}
	
	if (!pathDiffuseTextureMap.empty()) {
		hasDiffuseTextureMap = true;
		initTexture(pathDiffuseTextureMap, diffuseTextureMapHandle);
	}

	if (!pathSpecularTextureMap.empty()) {
		hasSpecularTextureMap = true;
		initTexture(pathSpecularTextureMap, specularTextureMapHandle);
	}

	if (!pathShininessTextureMap.empty()) {
		hasShininessTextureMap = true;
		initTexture(pathShininessTextureMap, shininessTextureMapHandle);
	}

	if (!pathAlphaTextureMap.empty()) {
		hasAlphaTextureMap = true;
		initTexture(pathAlphaTextureMap, alphaTextureMapHandle);
	}
}

Material::~Material() {

}

void Material::initTexture(std::string pathTextureMap, GLuint &textureMapHandle) {
	glGenTextures(1, &textureMapHandle);
	glBindTexture(GL_TEXTURE_2D, textureMapHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(pathTextureMap.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture from: " + pathTextureMap << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	stbi_image_free(data);
}

void Material::setTextures() {
	if (hasAmbientTextureMap) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ambientTextureMapHandle);
	}
	
	if (hasDiffuseTextureMap) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseTextureMapHandle);
	}

	if (hasSpecularTextureMap) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specularTextureMapHandle);
	}

	if (hasShininessTextureMap) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shininessTextureMapHandle);
	}

	if (hasAlphaTextureMap) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, alphaTextureMapHandle);
	}
}

void Material::setUniforms() {
	shader->setUniform("material.ambientColor", ambientColor);
	shader->setUniform("material.diffuseColor", diffuseColor);
	shader->setUniform("material.specularColor", specularColor);
	shader->setUniform("material.shininess", shininess);
	shader->setUniform("material.alpha", alpha);
	
	if (hasAmbientTextureMap) shader->setUniform("material.ambientTextureMapUnit", 0);
	if (hasDiffuseTextureMap) shader->setUniform("material.diffuseTextureMapUnit", 1);
	if (hasSpecularTextureMap) shader->setUniform("material.specularTextureMapUnit", 2);
	if (hasShininessTextureMap) shader->setUniform("material.shininessTextureMapUnit", 3);
	if (hasAlphaTextureMap) shader->setUniform("material.alphaTextureMapUnit", 4);
}
