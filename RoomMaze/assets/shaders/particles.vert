#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec3 position;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

/* ----- */
// MAIN
/* ----- */

void main() {
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
