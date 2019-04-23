#include "Geometry.h"
#include "LightCollection.h"

Geometry::Geometry(GeometryData &geometryData, glm::mat4 modelMatrix, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material)
	: elements(geometryData.indices.size()), modelMatrix(modelMatrix), shader(shader), material(material) {

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, geometryData.vertices.size() * sizeof(glm::vec3), geometryData.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// VBO for normals
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, geometryData.normals.size() * sizeof(glm::vec3), geometryData.normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// VBO for indices
	glGenBuffers(1, &vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometryData.indices.size() * sizeof(unsigned int), geometryData.indices.data(), GL_STATIC_DRAW);

	// VBO for UV coords
	glGenBuffers(1, &vboUVCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboUVCoords);
	glBufferData(GL_ARRAY_BUFFER, geometryData.UVCoords.size() * sizeof(glm::vec2), geometryData.UVCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// VBO for tangents
	glGenBuffers(1, &vboTangents);
	glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
	glBufferData(GL_ARRAY_BUFFER, geometryData.tangents.size() * sizeof(glm::vec3), geometryData.tangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// VBO for bitangents
	glGenBuffers(1, &vboBitangents);
	glBindBuffer(GL_ARRAY_BUFFER, vboBitangents);
	glBufferData(GL_ARRAY_BUFFER, geometryData.bitangents.size() * sizeof(glm::vec3), geometryData.bitangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry() {
	
}

void Geometry::destroy() {
	glDeleteBuffers(1, &vboVertices);
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboIndices);
	glDeleteBuffers(1, &vboUVCoords);
	glDeleteBuffers(1, &vboTangents);
	glDeleteBuffers(1, &vboBitangents);
	glDeleteVertexArrays(1, &vao);
}

void Geometry::setUniformsAndDraw(Camera &camera) {
	shader->use();

	shader->setUniform("modelMatrix", modelMatrix);
	
	camera.setUniforms(shader);

	material->setUniforms(shader);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	shader->unuse();
}

void Geometry::transform(glm::mat4 transformation) {
	modelMatrix = transformation * modelMatrix;
}

void Geometry::resetModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
}
