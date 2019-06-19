#version 430 core

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in vec2 UVCoords;

out vec4 color;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;

/* ----- */
// MAIN
/* ----- */

void main() {
    vec3 hdrColor = texture(scene, UVCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, UVCoords).rgb;

    if (bloom)
        hdrColor += bloomColor; // additive blending
    
	// tone mapping
    color = vec4(vec3(1.0) - exp(-hdrColor * /*exposure*/1.0), 1.0);
}
