#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <memory>
#include <physx\PxPhysicsAPI.h>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"

class Component3D {
protected:

	GLuint vao;

	GLuint vboVertices;
	GLuint vboNormals;
	GLuint vboIndices;
	GLuint vboUVCoords;
	GLuint vboTangents;

	// number of elements (faces) to be rendered
	unsigned int elements;

	// material of the component
	std::shared_ptr<Material> material;

	// physX shape
	physx::PxShape *pxShape;
public:
	// stores geometry data to construct a component
	struct GeometryData {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;
		std::vector<glm::vec2> UVCoords;
		std::vector<glm::vec3> tangents;
	};

	/**
	 * CONSTRUCTOR
	 * Creates VAO and VBOs and binds them.
	 *
	 * @param geometryData: stores geometry data to construct a component
	 * @param material: material of the component
	 */
	Component3D(GeometryData &geometryData, std::shared_ptr<Material> material);

	~Component3D();

	// Delete VAO and VBOs.
	void destroy();

	/**
	 * Sets the uniforms of the Material and draws the component.
	 *
	 * @param shader: Shader used to render the component
	 */
	void draw(std::shared_ptr<Shader> shader);
};
