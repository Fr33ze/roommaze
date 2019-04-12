#pragma once

#include <iostream>
#include <sstream>

#include <glm\glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>

#include "stb_image.h"
#include "Shader.h"

class Material {

protected:

	// shader used for rendering this material
	std::shared_ptr<Shader> shader;

	// Ka
	glm::vec3 ambientColor;
	// Kd
	glm::vec3 diffuseColor;
	// Ks
	glm::vec3 specularColor;
	// Ns
	float shininess;
	// Tr = 1 - d (fully opaque if Tr == 0.0 || d == 1.0)
	float alpha;

	// map_Ka
	bool hasAmbientTextureMap = false;
	GLuint ambientTextureMapHandle;
	// map_Kd
	bool hasDiffuseTextureMap = false;
	GLuint diffuseTextureMapHandle;
	// map_Ks
	bool hasSpecularTextureMap = false;
	GLuint specularTextureMapHandle;
	// map_Ns
	bool hasShininessTextureMap = false;
	GLuint shininessTextureMapHandle;
	// map_d
	bool hasAlphaTextureMap = false;
	GLuint alphaTextureMapHandle;

	void initTexture(std::string pathTextureMap, GLuint &textureMapHandle);

public:

	/**
	 * CONSTRUCTOR
	 * Creates a material.
	 */
	Material(std::shared_ptr<Shader> shader, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float alpha, std::string pathAmbientTextureMap, std::string pathDiffuseTextureMap, std::string pathSpecularTextureMap, std::string pathShininessTextureMap, std::string pathAlphaTextureMap);

	~Material();

	// Activates and binds material's textures.
	void setTextures();

	// Sets material's parameters as uniforms in its specified shader.
	void setUniforms();

};
