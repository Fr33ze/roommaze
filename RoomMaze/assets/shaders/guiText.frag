#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in vec2 UVCoords;

out vec4 color;

uniform float sceneBrightness;
uniform sampler2D textureUnit;
uniform float alpha;

/* ----- */
// MAIN
/* ----- */

void main() {
	float alphaChannel = alpha * texture(textureUnit, UVCoords).r;
	if (alphaChannel < 0.1)
		discard;
	color = vec4(vec3(1.0) * sceneBrightness, alphaChannel);
}
