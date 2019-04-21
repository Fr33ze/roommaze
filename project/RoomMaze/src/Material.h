#pragma once

#include <iostream>
#include <sstream>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "stb_image.h"
#include "Shader.h"

class Material {

protected:

	// Ka
	glm::vec3 ambientColor;
	// Kd
	glm::vec3 diffuseColor;
	// Ks
	glm::vec3 specularColor;
	// Ns
	float shininess;
	// d (fully opaque if d == 1.0)
	float alpha;

	// map_Kd
	bool hasDiffuseTextureMap = false;
	GLuint diffuseTextureMapHandle;
	// map_Ks
	bool hasSpecularTextureMap = false;
	GLuint specularTextureMapHandle;
	// map_d
	bool hasAlphaTextureMap = false;
	GLuint alphaTextureMapHandle;
	// map_n
	bool hasNormalTextureMap = false;
	GLuint normalTextureMapHandle;

	void initTexture(std::string pathTextureMap, GLuint &textureMapHandle);

public:

	/**
	 * CONSTRUCTOR
	 * Creates a material.
	 *
	 * @param ambientColor: Ka
	 * @param diffuseColor: Kd
	 * @param specularColor: Ks
	 * @param shininess: Ns
	 * @param alpha: d
	 * @param pathDiffuseTextureMap: map_Kd (RGB)
	 * @param pathSpecularTextureMap: map_Ks (R)
	 * @param pathAlphaTextureMap: map_d (RGBA)
	 * @param pathNormalMap: map_n (RGB)
	 */
	Material(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float alpha, std::string pathDiffuseTextureMap, std::string pathSpecularTextureMap, std::string pathAlphaTextureMap, std::string pathNormalTextureMap);

	~Material();

	/**
	 * Sets material's parameters as uniforms in the given shader (also activates and binds material's textures).
	 *
	 * @param shader: shader used for rendering
	 */
	void setUniforms(std::shared_ptr<Shader> shader);

};
