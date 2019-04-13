#include "Geometry.h"

Geometry::Geometry(GeometryData &geometryData, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material, std::shared_ptr<LightCollection> lightCollection, glm::mat4 modelMatrix)
	: elements(geometryData.indices.size()), shader(shader), material(material), lightCollection(lightCollection), modelMatrix(modelMatrix) {

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
	shader->setUniform("viewMatrix", camera.getViewMatrix());
	shader->setUniform("projectionMatrix", camera.getProjectionMatrix());
	shader->setUniform("cameraPosition", camera.getPosition());

	material->setTextures();
	material->setUniforms();

	lightCollection->setUniforms();

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

GeometryData Geometry::createCube(float width, float height, float depth) {
	GeometryData geometryData;

	/* --------------------------------------------- */
	// VERTEX POSITIONS
	/* --------------------------------------------- */

	float x = width / 2.0f;
	float y = height / 2.0f;
	float z = depth / 2.0f;

	// right
	geometryData.vertexPositions.push_back(glm::vec3(x, y, z)); // 0
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, z)); // 1
	geometryData.vertexPositions.push_back(glm::vec3(x, y, -z)); // 2
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, -z)); // 3
	// back
	geometryData.vertexPositions.push_back(glm::vec3(x, y, -z)); // 4
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, -z)); // 5
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, -z)); // 6
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, -z)); // 7
	// left
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, -z)); // 8
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, -z)); // 9
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, z)); // 10
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, z)); // 11
	// front
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, z)); // 12
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, z)); // 13
	geometryData.vertexPositions.push_back(glm::vec3(x, y, z)); // 14
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, z)); // 15
	// top
	geometryData.vertexPositions.push_back(glm::vec3(x, y, z)); // 16
	geometryData.vertexPositions.push_back(glm::vec3(x, y, -z)); // 17
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, -z)); // 18
	geometryData.vertexPositions.push_back(glm::vec3(-x, y, z)); // 19
	// bottom
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, z)); // 20
	geometryData.vertexPositions.push_back(glm::vec3(x, -y, -z)); // 21
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, -z)); // 22
	geometryData.vertexPositions.push_back(glm::vec3(-x, -y, z)); // 23

	/* --------------------------------------------- */
	// NORMALS
	/* --------------------------------------------- */

	// right
	geometryData.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	// back
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	// left
	geometryData.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	// front
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	// top
	geometryData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	// bottom
	geometryData.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	geometryData.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

	/* --------------------------------------------- */
	// INDICES
	/* --------------------------------------------- */

	// right
	geometryData.indices.push_back(0);
	geometryData.indices.push_back(1);
	geometryData.indices.push_back(3);
	geometryData.indices.push_back(3);
	geometryData.indices.push_back(2);
	geometryData.indices.push_back(0);
	// back
	geometryData.indices.push_back(4);
	geometryData.indices.push_back(5);
	geometryData.indices.push_back(7);
	geometryData.indices.push_back(7);
	geometryData.indices.push_back(6);
	geometryData.indices.push_back(4);
	// left
	geometryData.indices.push_back(8);
	geometryData.indices.push_back(9);
	geometryData.indices.push_back(11);
	geometryData.indices.push_back(11);
	geometryData.indices.push_back(10);
	geometryData.indices.push_back(8);
	// front
	geometryData.indices.push_back(12);
	geometryData.indices.push_back(13);
	geometryData.indices.push_back(15);
	geometryData.indices.push_back(15);
	geometryData.indices.push_back(14);
	geometryData.indices.push_back(12);
	// top
	geometryData.indices.push_back(18);
	geometryData.indices.push_back(19);
	geometryData.indices.push_back(16);
	geometryData.indices.push_back(16);
	geometryData.indices.push_back(17);
	geometryData.indices.push_back(18);
	// bottom
	geometryData.indices.push_back(23);
	geometryData.indices.push_back(22);
	geometryData.indices.push_back(21);
	geometryData.indices.push_back(21);
	geometryData.indices.push_back(20);
	geometryData.indices.push_back(23);

	/* --------------------------------------------- */
	// UV COORDS
	/* --------------------------------------------- */

	// right, back, left, front
	for (int i = 0; i < 4; i++) {
		geometryData.UVCoords.push_back(glm::vec2(0.0f, 1.0f));
		geometryData.UVCoords.push_back(glm::vec2(0.0f, 0.0f));
		geometryData.UVCoords.push_back(glm::vec2(1.0f, 1.0f));
		geometryData.UVCoords.push_back(glm::vec2(1.0f, 0.0f));
	}
	// top
	geometryData.UVCoords.push_back(glm::vec2(1.0f, 0.0f));
	geometryData.UVCoords.push_back(glm::vec2(1.0f, 1.0f));
	geometryData.UVCoords.push_back(glm::vec2(0.0f, 1.0f));
	geometryData.UVCoords.push_back(glm::vec2(0.0f, 0.0f));
	// bottom
	geometryData.UVCoords.push_back(glm::vec2(1.0f, 1.0f));
	geometryData.UVCoords.push_back(glm::vec2(1.0f, 0.0f));
	geometryData.UVCoords.push_back(glm::vec2(0.0f, 0.0f));
	geometryData.UVCoords.push_back(glm::vec2(0.0f, 1.0f));

	return geometryData;
}
