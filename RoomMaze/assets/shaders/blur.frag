#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in vec2 UVCoords;

out vec4 color;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

/* ----- */
// MAIN
/* ----- */

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
    
	vec3 result = texture(image, UVCoords).rgb * weight[0];

	if (horizontal) {
		for(int i = 1; i < 5; ++i) {
			result += texture(image, UVCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, UVCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		}
    } else {
		for(int i = 1; i < 5; ++i) {
             result += texture(image, UVCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
             result += texture(image, UVCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
		}
	}
    color = vec4(result, 1.0);
}
