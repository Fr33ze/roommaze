#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "LightCollection.h"

// stores all data for a geometry object
struct GeometryData {
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> UVCoords;
};

/* -------------------------------------------------- */
// VARIABLES & METHODS
/* -------------------------------------------------- */

class Geometry {

protected:

	GLuint vao;

	GLuint vboVertexPositions;
	GLuint vboNormals;
	GLuint vboIndices;
	GLuint vboUVCoords;

	// number of elements (faces) to be rendered
	unsigned int elements;

	// shader used for rendering this object
	std::shared_ptr<Shader> shader;

	// material of the object
	std::shared_ptr<Material> material;

	// light collection of the object
	std::shared_ptr<LightCollection> lightCollection;

	// model matrix of the object
	glm::mat4 modelMatrix;

public:

	/**
	 * CONSTRUCTOR
	 * Creates VAO and VBOs and binds them.
	 *
	 * @param geometryData: geometryData for the object
	 * @param shader: shader used for rendering this object
	 * @param material: material of the object
	 * @param lightCollection: light collection of the object
	 * @param modelMatrix: model matrix of the object
	 */
	Geometry(GeometryData &geometryData, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material, std::shared_ptr<LightCollection> lightCollection, glm::mat4 modelMatrix);

	Geometry();

	~Geometry();

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

	/**
	 * Creates a cube geometry.
	 *
	 * @param width: width of the cube
	 * @param height: height of the cube
	 * @param depth: depth of the cube
	 *
	 * @return all cube data
	 */
	static GeometryData createCube(float width, float height, float depth);

};
