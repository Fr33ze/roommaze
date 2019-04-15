#version 430 core

/* -------- */
// STRUCTS
/* -------- */

struct Camera {
	vec3 position;

	// light stuff
	vec3 direction;
	vec3 intensity;
	float innerCutOff;
	float outerCutOff;
	vec3 attenuation;
};

struct Material {
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
	float alpha;

	bool hasAmbientTextureMap;
	sampler2D ambientTextureMapUnit;
	bool hasDiffuseTextureMap;
	sampler2D diffuseTextureMapUnit;
	bool hasSpecularTextureMap;
	sampler2D specularTextureMapUnit;
	bool hasAlphaTextureMap;
	sampler2D alphaTextureMapUnit;
	bool hasNormalTextureMap;
	sampler2D normalTextureMapUnit;
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

/* ------------------------------ */
// UNIFORMS, IN- & OUT-VARIABLES
/* ------------------------------ */

uniform Camera camera;

uniform Material material;

uniform DirectionalLight directionalLights[10];
uniform uint amountOfDirectionalLights;
uniform PointLight pointLights[10];
uniform uint amountOfPointLights;
uniform SpotLight spotLights[10];
uniform uint amountOfSpotLights;

in VertexData {
	vec3 positionWorld;
	vec3 normal;
	vec2 UVCoords;
} vertexData;

out vec4 color;

/* ----------- */
// PROTOTYPES
/* ----------- */

vec3 calculateDirectionalLight(int i);
vec3 calculatePointLight(int i);
vec3 calculateSpotLight(int i);
vec3 calculateCameraLight();

/* ----- */
// MAIN
/* ----- */

void main() {

	// AMBIENT LIGHT (ambientColor x ambientTexture)
	// =============================================
	vec3 ambientLight =  material.hasAmbientTextureMap ? material.ambientColor * texture(material.ambientTextureMapUnit, vertexData.UVCoords).rgb : material.ambientColor;

	// LIGHT & COLOR
	// =============
	vec3 directionalLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfDirectionalLights; i++)
		directionalLight += calculateDirectionalLight(i);

	vec3 pointLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfPointLights; i++)
		pointLight += calculatePointLight(i);

	vec3 spotLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfSpotLights; i++)
		spotLight += calculateSpotLight(i);

	vec3 light = ambientLight + directionalLight + pointLight + spotLight + calculateCameraLight();
	float alphaChannel = material.hasAlphaTextureMap ? material.alpha * texture(material.alphaTextureMapUnit, vertexData.UVCoords).r : material.alpha;
	if (alphaChannel < 0.1)
		discard;
	color = vec4(light, alphaChannel);
}

vec3 calculateDirectionalLight(int i) {

	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// =========================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(-directionalLights[i].direction);

	vec3 diffuseLight = directionalLights[i].color * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// =================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = directionalLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// TEXTURING
	// =========
	return (material.hasDiffuseTextureMap ? diffuseLight * texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : diffuseLight) + (material.hasSpecularTextureMap ? specularLight * texture(material.specularTextureMapUnit, vertexData.UVCoords).r : specularLight);
}

vec3 calculatePointLight(int i) {

	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// =========================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(pointLights[i].position - vertexData.positionWorld);

	vec3 diffuseLight = pointLights[i].color * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// =================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = pointLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(pointLights[i].position - vertexData.positionWorld);
	float attenuation = 1.0 / (pointLights[i].attenuation[0] + (pointLights[i].attenuation[1] * distanceToLight) + (pointLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

	// TEXTURING
	// =========
	return attenuation * ((material.hasDiffuseTextureMap ? diffuseLight * texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : diffuseLight) + (material.hasSpecularTextureMap ? specularLight * texture(material.specularTextureMapUnit, vertexData.UVCoords).r : specularLight));
}

vec3 calculateSpotLight(int i) {
		
	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// =========================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(spotLights[i].position - vertexData.positionWorld);

	vec3 diffuseLight = spotLights[i].intensity * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// =================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = spotLights[i].intensity * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(spotLights[i].position - vertexData.positionWorld);
	float attenuation = 1.0 / (spotLights[i].attenuation[0] + (spotLights[i].attenuation[1] * distanceToLight) + (spotLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

	// check if inside light cone and if so calculate a smooth light cone edge
	// =======================================================================
	float theta = dot(normalizedLightDirection, normalize(-spotLights[i].direction));
	float epsilon = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
	float coneIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0); // if outside only black color will be returned

	// TEXTURING
	// =========
	return coneIntensity * attenuation * ((material.hasDiffuseTextureMap ? diffuseLight * texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : diffuseLight) + (material.hasSpecularTextureMap ? specularLight * texture(material.specularTextureMapUnit, vertexData.UVCoords).r : specularLight));
}

vec3 calculateCameraLight() {
	// DIFFUSE LIGHT (lightIntensity x diffuseColor x (normal x lightDirection))
	// =========================================================================
	vec3 normalizedNormal = normalize(vertexData.normal);
	vec3 normalizedLightDirection = normalize(camera.position - vertexData.positionWorld);

	vec3 diffuseLight = camera.intensity * material.diffuseColor * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x specularColor x (viewDirection x reflectionDirection)^shininess)
	// =================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	vec3 specularLight = camera.intensity * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(camera.position - vertexData.positionWorld);
	float attenuation = 1.0 / (camera.attenuation[0] + (camera.attenuation[1] * distanceToLight) + (camera.attenuation[2] * (distanceToLight * distanceToLight)));

	// check if inside light cone and if so calculate a smooth light cone edge
	// =======================================================================
	float theta = dot(normalizedLightDirection, normalize(camera.direction));
	float epsilon = camera.innerCutOff - camera.outerCutOff;
	float coneIntensity = clamp((theta - camera.outerCutOff) / epsilon, 0.0, 1.0); // if outside only black color will be returned

	// TEXTURING
	// =========
	return coneIntensity * attenuation * ((material.hasDiffuseTextureMap ? diffuseLight * texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : diffuseLight) + (material.hasSpecularTextureMap ? specularLight * texture(material.specularTextureMapUnit, vertexData.UVCoords).r : specularLight));
}
