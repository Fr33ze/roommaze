#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec2 vertices;
layout(location = 1) in vec2 UVs;
layout(location = 2) in vec4 xyzs;

out vec2 UVCoords;

uniform vec3 cameraRightWorldspace;
uniform vec3 cameraUpWorldspace;
uniform mat4 viewProjectionMatrix;

/* ----- */
// MAIN
/* ----- */

void main() {

	vec3 particleCenterWordspace = xyzs.xyz;
	float particleSize = xyzs.w;
	
	vec3 vertexPositionWorldspace = particleCenterWordspace + cameraRightWorldspace * vertices.x * particleSize + cameraUpWorldspace * vertices.y * particleSize;
	
	gl_Position = viewProjectionMatrix  * vec4(vertexPositionWorldspace, 1.0);

	UVCoords = UVs;
}
