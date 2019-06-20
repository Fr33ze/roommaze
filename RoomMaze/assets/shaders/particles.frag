#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in vec2 UVCoords;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform float sceneBrightness;
uniform float particleBrightness;
uniform sampler2D textureUnit;

/* ----- */
// MAIN
/* ----- */

void main() {
	float alphaChannel = texture(textureUnit, UVCoords).a;
	if (alphaChannel < 0.1)
		discard;
	vec3 result = texture(textureUnit, UVCoords).rgb * particleBrightness;
	color = vec4(result * sceneBrightness * particleBrightness, alphaChannel);

	// check whether color is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        brightColor = vec4(result, 1.0);
	else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
