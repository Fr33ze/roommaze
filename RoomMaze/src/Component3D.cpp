#include "Component3D.h"
#include "LightCollection.h"

Component3D::Component3D(GeometryData &geometryData, std::shared_ptr<Material> material)
	: elements(geometryData.indices.size()), material(material) {

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
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// unbind VAO
	glBindVertexArray(0);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Component3D::~Component3D() {

}

void Component3D::destroy() {
	glDeleteBuffers(1, &vboVertices);
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboIndices);
	glDeleteBuffers(1, &vboUVCoords);
	glDeleteBuffers(1, &vboTangents);
	glDeleteVertexArrays(1, &vao);
}

void Component3D::draw(std::shared_ptr<Shader> shader) {
	material->setUniforms(shader);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}