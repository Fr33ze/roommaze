#version 430 core

#define AMOUNT_OF_DIRECTIONAL_LIGHTS 1
#define AMOUNT_OF_POINT_LIGHTS 0
#define AMOUNT_OF_SPOT_LIGHTS 0

/* -------------------------------------------------- */
// STRUCTS
/* -------------------------------------------------- */

struct Material {
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
	float alpha;

	sampler2D ambientTextureMapUnit;
	sampler2D diffuseTextureMapUnit;
	sampler2D specularTextureMapUnit;
	sampler2D shininessTextureMapUnit;
	sampler2D alphaTextureMapUnit;
};

struct DirectionalLight {
	vec3 color;
	vec3 direction;
};

struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 intensity;
	float innerCutOff;
	float outerCutOff;
	vec3 attenuation;
};

/* -------------------------------------------------- */
// UNIFORMS, IN- & OUT-VARIABLES
/* -------------------------------------------------- */

uniform vec3 cameraPosition;

uniform Material material;

uniform DirectionalLight directionalLights[10];
uniform PointLight pointLights[10];
uniform SpotLight spotLights[10];

in VertexData {
	vec3 positionWorld;
	vec3 normal;
	vec2 UVCoords;
} vertexData;

out vec4 color;

/* -------------------------------------------------- */
// PROTOTYPES
/* -------------------------------------------------- */

vec3 calculateDirectionalLight(int i);
vec3 calculatePointLight(int i);
vec3 calculateSpotLight(int i);

/* -------------------------------------------------- */
// MAIN
/* -------------------------------------------------- */

void main() {

	// AMBIENT LIGHT (ambientIntensity x ambientColor)
	// ============================================================
	vec3 ambientIntensity = vec3(1.0, 1.0, 1.0);
	vec3 ambientLight =  ambientIntensity * material.ambientColor;
	ambientLight = texture(material.ambientTextureMapUnit, vertexData.UVCoords).rgb * ambientLight;

	// LIGHT & COLOR
	// =============
	vec3 directionalLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < AMOUNT_OF_DIRECTIONAL_LIGHTS; i++)
		directionalLight += calculateDirectionalLight(i);

	vec3 pointLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < AMOUNT_OF_POINT_LIGHTS; i++)
		pointLight += calculatePointLight(i);

	vec3 spotLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < AMOUNT_OF_SPOT_LIGHTS; i++)
		spotLight += calculateSpotLight(i);

	vec3 light = ambientLight + directionalLight + pointLight + spotLight;
	color = vec4(light, 1.0);
}

vec3 calculateDirectionalLight(int i) {

	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// ======================================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(-directionalLights[i].direction);

	vec3 diffuseLight = directionalLights[i].color * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// ======================================================================================================================
	vec3 normalizedViewDirection = normalize(cameraPosition - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = directionalLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	return (texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb * diffuseLight) + (texture(material.specularTextureMapUnit, vertexData.UVCoords).rgb * specularLight);
}

vec3 calculatePointLight(int i) {

	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// ======================================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(pointLights[i].position - vertexData.positionWorld);

	vec3 diffuseLight = pointLights[i].color * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// ======================================================================================================================
	vec3 normalizedViewDirection = normalize(cameraPosition - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = pointLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(pointLights[i].position - vertexData.positionWorld);
	float attenuation = 1.0 / (pointLights[i].attenuation[0] + (pointLights[i].attenuation[1] * distanceToLight) + (pointLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

	return attenuation * ((texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb * diffuseLight) + (texture(material.specularTextureMapUnit, vertexData.UVCoords).rgb * specularLight));
}

vec3 calculateSpotLight(int i) {

	// check if inside light cone
	// ==========================
	vec3 normalizedLightDirection = normalize(spotLights[i].position - vertexData.positionWorld);
	float theta = dot(normalizedLightDirection, normalize(-spotLights[i].direction));
	if (theta > spotLights[i].outerCutOff) {
		
		// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
		// ======================================================================================
		vec3 normalizedNormal = normalize(vertexData.normal);

		vec3 diffuseLight = spotLights[i].intensity * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

		// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
		// ======================================================================================================================
		vec3 normalizedViewDirection = normalize(cameraPosition - vertexData.positionWorld);
		vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

		vec3 specularLight = spotLights[i].intensity * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

		// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
		// ===============================================================================================================
		float distanceToLight = length(spotLights[i].position - vertexData.positionWorld);
		float attenuation = 1.0 / (spotLights[i].attenuation[0] + (spotLights[i].attenuation[1] * distanceToLight) + (spotLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

		// some calculations for a smooth light cone edge
		// ==============================================
		float epsilon = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
		float coneIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

		return coneIntensity * attenuation * ((texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb * diffuseLight) + (texture(material.specularTextureMapUnit, vertexData.UVCoords).rgb * specularLight));
	}

	// return black light if outside light cone
	// ========================================
	return vec3(0.0, 0.0, 0.0);
}
