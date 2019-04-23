#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"

class Geometry {

protected:

	GLuint vao;

	GLuint vboVertices;
	GLuint vboNormals;
	GLuint vboIndices;
	GLuint vboUVCoords;
	GLuint vboTangents;
	GLuint vboBitangents;

	// number of elements (faces) to be rendered
	unsigned int elements;

	// model matrix of the object
	glm::mat4 modelMatrix;

	// shader used for rendering
	std::shared_ptr<Shader> shader;

	// material of the object
	std::shared_ptr<Material> material;

public:

	// stores all data for a geometry object
	struct GeometryData {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;
		std::vector<glm::vec2> UVCoords;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
	};

	/**
	 * CONSTRUCTOR
	 * Creates VAO and VBOs and binds them.
	 *
	 * @param geometryData: stores all data for a geometry object
	 * @param modelMatrix: model matrix of the object
	 * @param shader: shader used for rendering
	 * @param material: material of the object
	 */
	Geometry(GeometryData &geometryData, glm::mat4 modelMatrix, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material);

	~Geometry();

	// Delete VAO and VBOs.
	void destroy();

	/**
	 * Sets the uniforms of the shader and draws the object.
	 *
	 * @param camera: camera of the scene that should be rendered
	 */
	void setUniformsAndDraw(Camera &camera);

	/**
	 * Transforms the object (updates the model matrix).
	 *
	 * @param transformation: the transformation matrix to be applied to the object
	 */
	void transform(glm::mat4 transformation);

	// Resets the model matrix to the identity matrix.
	void resetModelMatrix();

};
