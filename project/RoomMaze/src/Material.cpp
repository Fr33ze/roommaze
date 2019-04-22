#include "Material.h"

Material::Material(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float alpha, std::string pathAmbientTextureMap, std::string pathDiffuseTextureMap, std::string pathSpecularTextureMap, std::string pathAlphaTextureMap, std::string pathNormalTextureMap)
	: ambientColor(ambientColor), diffuseColor(diffuseColor), specularColor(specularColor), shininess(shininess), alpha(alpha) {
	
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

	if (!pathAlphaTextureMap.empty()) {
		hasAlphaTextureMap = true;
		initTexture(pathAlphaTextureMap, alphaTextureMapHandle);
	}

	if (!pathNormalTextureMap.empty()) {
		hasNormalTextureMap = true;
		initTexture(pathNormalTextureMap, normalTextureMapHandle);
	}
}

Material::~Material() {

}

void Material::initTexture(std::string pathTextureMap, GLuint &textureMapHandle) {
	glGenTextures(1, &textureMapHandle);
	glBindTexture(GL_TEXTURE_2D, textureMapHandle);

	// load texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(pathTextureMap.c_str(), &width, &height, &nrChannels, 0);
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
	}
	else {
		std::cout << "Failed to load texture from: " + pathTextureMap << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	stbi_image_free(data);
}

void Material::setUniforms(std::shared_ptr<Shader> shader) {
	shader->setUniform("material.ambientColor", ambientColor);
	shader->setUniform("material.diffuseColor", diffuseColor);
	shader->setUniform("material.specularColor", specularColor);
	shader->setUniform("material.shininess", shininess);
	shader->setUniform("material.alpha", alpha);

	if (hasAmbientTextureMap) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ambientTextureMapHandle);
		shader->setUniform("material.hasAmbientTextureMap", true);
		shader->setUniform("material.ambientTextureMapUnit", 0);
	}
	else {
		shader->setUniform("material.hasAmbientTextureMap", false);
	}

	if (hasDiffuseTextureMap) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseTextureMapHandle);
		shader->setUniform("material.hasDiffuseTextureMap", true);
		shader->setUniform("material.diffuseTextureMapUnit", 1);
	} else {
		shader->setUniform("material.hasDiffuseTextureMap", false);
	}

	if (hasSpecularTextureMap) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specularTextureMapHandle);
		shader->setUniform("material.hasSpecularTextureMap", true);
		shader->setUniform("material.specularTextureMapUnit", 2);
	} else {
		shader->setUniform("material.hasSpecularTextureMap", false);
	}

	if (alpha > 0.0f || hasAlphaTextureMap) {
		glDisable(GL_CULL_FACE); // FACE CULLING = render only the front side of every face (disable for correct blending)
		glEnable(GL_BLEND); // enable rendering semi-transparent materials
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how blendig is accomplished
	} else {
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}

	if (hasAlphaTextureMap) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, alphaTextureMapHandle);
		shader->setUniform("material.hasAlphaTextureMap", true);
		shader->setUniform("material.alphaTextureMapUnit", 3);
	} else {
		shader->setUniform("material.hasAlphaTextureMap", false);
	}

	if (hasNormalTextureMap) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, normalTextureMapHandle);
		shader->setUniform("material.hasNormalTextureMap", true);
		shader->setUniform("material.normalTextureMapUnit", 4);
	} else {
		shader->setUniform("material.hasNormalTextureMap", false);
	}
}
