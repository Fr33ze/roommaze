#include "Geometry.h"

Geometry::Geometry(GeometryData &geometryData, glm::mat4 modelMatrix, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material)
	: elements(geometryData.indices.size()), modelMatrix(modelMatrix), shader(shader), material(material) {

	/* --------------------------- */
	// VAO (VERTEX BUFFER OBJECT)
	/* --------------------------- */

	// create one VAO and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* ----------------------------- */
	// VBOs (VERTEX BUFFER OBJECTs)
	/* ----------------------------- */

	// create one VBO for vertex positions
	glGenBuffers(1, &vboVertexPositions);

	/* http://docs.gl/gl4/glBindBuffer
		GL_ARRAY_BUFFER = vertex attributes */
	glBindBuffer(GL_ARRAY_BUFFER, vboVertexPositions);

	/* http://docs.gl/gl4/glBufferData
		GL_STATIC_DRAW = {STATIC = The data store contents will be modified once and used many times.}_{DRAW/READ/COPY} */
	glBufferData(GL_ARRAY_BUFFER, geometryData.vertexPositions.size() * sizeof(glm::vec3), geometryData.vertexPositions.data(), GL_STATIC_DRAW); // "geometryData.positions.data()" or "&geometryData.positions[0]"

	// Enable attribute index 0 as being used.
	glEnableVertexAttribArray(0);

	/* http://docs.gl/gl4/glVertexAttribPointer
		Specify that our coordinate data is going into attribute index 0, and contains three floats per vertex.

		Parameter 4 could be also 0 because there's no stride (only necessary if we store more than one thing
		in one vbo, for example vertex positions and their normals, then we need:

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 3 * sizeof(float));

		Parameter 5 defines where to start, so vertex positions are (1, 2, 3), (7, 8, 9), (13, 14, 15) ...
		and normals are (4, 5, 6), (10, 11, 12), (16, 17, 18) ...												*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// create one VBO for normals
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, geometryData.normals.size() * sizeof(glm::vec3), geometryData.normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	// create one VBO for indices
	glGenBuffers(1, &vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometryData.indices.size() * sizeof(unsigned int), geometryData.indices.data(), GL_STATIC_DRAW);

	// create one VBO for UV coords
	glGenBuffers(1, &vboUVCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboUVCoords);
	glBufferData(GL_ARRAY_BUFFER, geometryData.UVCoords.size() * sizeof(glm::vec2), geometryData.UVCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	/* ---------- */
	// UNBINDING
	/* ---------- */

	// unbind VAO
	glBindVertexArray(0);
	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry() {
	
}

void Geometry::destroy() {
	glDeleteBuffers(1, &vboVertexPositions);
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboIndices);
	glDeleteBuffers(1, &vboUVCoords);
	glDeleteVertexArrays(1, &vao);
}

void Geometry::setUniformsAndDraw(Camera &camera) {
	shader->use();

	shader->setUniform("modelMatrix", modelMatrix);
	
	camera.setUniforms(shader);

	material->setTextures();
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
