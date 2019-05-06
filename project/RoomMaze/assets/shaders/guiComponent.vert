#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec2 position;

out vec2 UVCoords;

uniform mat4 modelMatrix;

/* ----- */
// MAIN
/* ----- */

void main() {
	gl_Position = modelMatrix * vec4(position, 0.0, 1.0);
	UVCoords = vec2((position.x + 1.0) / 2.0, 1.0 - (position.y + 1.0) / 2.0);
}
