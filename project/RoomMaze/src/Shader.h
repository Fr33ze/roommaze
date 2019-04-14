#pragma once

#include <iostream>
#include <sstream>

#include <GL\glew.h>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <glm\gtc\type_ptr.hpp>

class Shader {

protected:

	// the shader's program handle
	GLuint program;

	/**
	 * Loads and compiles the shader.
	 *
	 * @param filePathVertexShader: path to the vertex shader
	 * @param filePathFragmentShader: path to the fragment shader
	 */
	void loadShader(std::string filePathVertexShader, std::string filePathFragmentShader);

	/**
	 * Compiles the shader.
	 *
	 * @param shaderType: type of the shader = {GL_VERTEX_SHADER/GL_FRAGMENT_SHADER}
	 * @param filePath: path to the shader
	 */
	GLuint compileShader(GLenum shaderType, std::string filePath);

	/**
	 * Reads the shader file from a given filePath.
	 *
	 * @param filePath: path to the shader
	 */
	std::string readFile(std::string filePath);

public:

	/**
	 * CONSTRUCTOR
	 * Loads and compiles the shader.
	 *
	 * @param filePathVertexShader: path to the vertex shader
	 * @param filePathFragmentShader: path to the fragment shader
	 */
	Shader(std::string filePathVertexShader, std::string filePathFragmentShader);

	~Shader();

	// Uses the shader with glUseProgram(program).
	void use();

	// Unuses the shader with glUseProgram(0).
	void unuse();

	/**
	 * Sets an integer uniform in the shader.
	 *
	 * @param uniform: the name of the uniform
	 * @param value: the value to be set
	 */
	void setUniform(std::string uniform, int value);

	/**
	 * Sets an unsigned integer uniform in the shader.
	 *
	 * @param uniform: the name of the uniform
	 * @param value: the value to be set
	 */
	void setUniform(std::string uniform, unsigned int value);

	/**
	 * Sets a float uniform in the shader.
	 *
	 * @param uniform: the name of the uniform
	 * @param value: the value to be set
	 */
	void setUniform(std::string uniform, float value);

	/**
	 * Sets a 3D vector uniform in the shader.
	 *
	 * @param uniform: the name of the uniform
	 * @param value: the value to be set
	 */
	void setUniform(std::string uniform, glm::vec3 value);

	/**
	 * Sets a 4x4 matrix uniform in the shader.
	 *
	 * @param uniform: the name of the uniform
	 * @param value: the value to be set
	 */
	void setUniform(std::string uniform, glm::mat4 value);

};
