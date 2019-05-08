#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in vec2 UVCoords;

out vec4 color;

uniform sampler2D textureUnit;

/* ----- */
// MAIN
/* ----- */

void main() {
	float alphaChannel = texture(textureUnit, UVCoords).a;
	if (alphaChannel < 0.1)
		discard;
	color = vec4(texture(textureUnit, UVCoords).rgb, alphaChannel);
}
