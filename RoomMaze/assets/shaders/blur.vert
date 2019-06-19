#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UVs;

out vec2 UVCoords;

/* ----- */
// MAIN
/* ----- */

void main() {
	gl_Position = vec4(position, 1.0);
	UVCoords = UVs;
}
