#include "Shader.h"

Shader::Shader(std::string pathVertexShader, std::string pathFragmentShader) {
	loadShader(pathVertexShader, pathFragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(program);
}

void Shader::loadShader(std::string filePathVertexShader, std::string filePathFragmentShader) {
	// generate shader objects
	GLuint vertexHandle = compileShader(GL_VERTEX_SHADER, filePathVertexShader);
	GLuint fragmentHandle = compileShader(GL_FRAGMENT_SHADER, filePathFragmentShader);

	// create program
	program = glCreateProgram();
	if (program == 0) {
		std::cout << "Failed to create shader program." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	// attache shader handles to program
	glAttachShader(program, vertexHandle);
	glAttachShader(program, fragmentHandle);

	// bind output
	glBindFragDataLocation(program, 0, "color");

	// link program
	glLinkProgram(program);

	// check program
	GLint succeded;
	glGetProgramiv(program, GL_LINK_STATUS, &succeded);
	if (succeded == GL_FALSE) {
		// read log and output it to std::cout
		GLint logSize;
		glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logSize);
		auto message = new char[logSize];
		glGetShaderInfoLog(program, logSize, nullptr, message);
		std::cout << "Failed to link shader program." << std::endl;
		std::cout << message << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		delete[] message;
		exit(-1);
	}

	// detache shaders after successful link
	glDetachShader(program, vertexHandle);
	glDetachShader(program, fragmentHandle);

	// delete handles
	glDeleteShader(vertexHandle);
	glDeleteShader(fragmentHandle);
}

GLuint Shader::compileShader(GLenum shaderType, std::string filePath) {
	// load shader file
	std::string code = readFile(filePath);
	
	// generate shader object
	GLuint handle = glCreateShader(shaderType);
	if (handle == 0) {
		std::cout << "Failed to create shader: " + filePath << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	// compile shader
	auto codePtr = code.c_str();
	glShaderSource(handle, 1, &codePtr, nullptr);
	glCompileShader(handle);

	// check shader
	GLint succeded;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE || !glIsShader(handle)) {
		// read log and output it to std::cout
		GLint logSize;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);
		auto message = new char[logSize];
		glGetShaderInfoLog(handle, logSize, nullptr, message);
		std::cout << "Failed to compile shader: " + filePath << std::endl;
		std::cout << message << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		delete[] message;
		exit(-1);
	}

	return handle;
}

std::string Shader::readFile(std::string filePath) {
	std::ifstream shaderFile(filePath);
	if (shaderFile.good()) {
		// read file into string
		std::string code = std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
		shaderFile.close();
		return code;
	} else {
		std::cout << "Failed to open shader file: " << filePath << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
}

void Shader::use() {
	glUseProgram(program);
}

void Shader::unuse() {
	glUseProgram(0);
}

void Shader::setUniform(std::string uniform, int value) {
	if (uniformLocations.find(uniform) == uniformLocations.end()) {
		uniformLocations.insert({ uniform, glGetUniformLocation(program, uniform.c_str()) });
	}
	glUniform1i(uniformLocations.at(uniform), value);
}

void Shader::setUniform(std::string uniform, unsigned int value) {
	if (uniformLocations.find(uniform) == uniformLocations.end()) {
		uniformLocations.insert({ uniform, glGetUniformLocation(program, uniform.c_str()) });
	}
	glUniform1ui(uniformLocations.at(uniform), value);
}

void Shader::setUniform(std::string uniform, float value) {
	if (uniformLocations.find(uniform) == uniformLocations.end()) {
		uniformLocations.insert({ uniform, glGetUniformLocation(program, uniform.c_str()) });
	}
	glUniform1f(uniformLocations.at(uniform), value);
}

void Shader::setUniform(std::string uniform, glm::vec3 value) {
	if (uniformLocations.find(uniform) == uniformLocations.end()) {
		uniformLocations.insert({ uniform, glGetUniformLocation(program, uniform.c_str()) });
	}
	glUniform3fv(uniformLocations.at(uniform), 1, glm::value_ptr(value));
}

void Shader::setUniform(std::string uniform, glm::mat4 value) {
	if (uniformLocations.find(uniform) == uniformLocations.end()) {
		uniformLocations.insert({ uniform, glGetUniformLocation(program, uniform.c_str()) });
	}
	glUniformMatrix4fv(uniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
}
