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
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out VertexData {
	vec3 positionWorld;
	vec3 normal;
	vec2 UVCoords;
	mat3 TBN;
} vertexData;

/* ----- */
// MAIN
/* ----- */

void main() {

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

	// Calculation in world space, not in view space (in view space also possible --> no need of the uniform cameraPosition).
	// ======================================================================================================================
	vertexData.positionWorld = vec3(modelMatrix * vec4(position, 1.0));

	// When transforming geometry with the model matrix, normals need to be transformed as well --> transformation with the inverse
	// transposed model matrix (also works for non-uniform scaling (= at least one of the scaling factors differs from the others)).
	// =============================================================================================================================
	vertexData.normal = mat3(transpose(inverse(modelMatrix))) * normal;

	vertexData.UVCoords = UVCoords;

	// TBN matrix (used to transform normals from tangent space (normals from a normal map) to a different space)
	// ==========================================================================================================
	vec3 N = normalize(vec3(modelMatrix * vec4(vertexData.normal, 0))); // working in world space --> multiplication with model matrix
	vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0)));
	// re-orthogonalize T with respect to N (Gram-Schmidt process)
	T = normalize(T - dot(T, N) * N);
	//vec3 B = normalize(cross(N, T));
	vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0)));
	vertexData.TBN = mat3(T, B, N);
}
