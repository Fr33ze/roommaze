#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

layout(location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

/* ----- */
// MAIN
/* ----- */

void main() {

    gl_Position = lightSpaceMatrix * modelMatrix * vec4(position, 1.0);
}  
