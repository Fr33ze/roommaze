#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec4 xyzs;

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
}
