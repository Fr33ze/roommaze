#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 UVs;

out vec2 UVCoords;

uniform mat4 projectionMatrix;

/* ----- */
// MAIN
/* ----- */

void main() {
	gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);
	UVCoords = UVs;
}
