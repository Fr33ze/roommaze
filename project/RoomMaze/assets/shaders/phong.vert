#version 430 core

/* ------------------------------ */
// UNIFORMS, IN- & OUT-VARIABLES
/* ------------------------------ */

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 UVCoords;

out VertexData {
	vec3 positionWorld;
	vec3 normal;
	vec2 UVCoords;
} vertexData;

/* ----- */
// MAIN
/* ----- */

void main() {

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

	// Calculation in world space, not in view space (in view space also possible -> no need of the uniform cameraPosition)).
	// ======================================================================================================================
	vertexData.positionWorld = vec3(modelMatrix * vec4(position, 1.0));

	// When transforming geometry with the model matrix, normals need to be transformed as well --> transformation with the inverse
	// transposed model matrix (also works for non-uniform scaling (= at least one of the scaling factors differs from the others)).
	// =============================================================================================================================
	vertexData.normal = mat3(transpose(inverse(modelMatrix))) * normal;

	vertexData.UVCoords = UVCoords;
}
