#version 430 core

/* -------- */
// STRUCTS
/* -------- */

struct Camera {
	vec3 position;

	// light stuff
	vec3 direction;
	float brightness;
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

/* ----------------------------- */
// IN-/OUT-VARIABLES & UNIFORMS
/* ----------------------------- */

in VertexData {
	vec3 positionWorld;
	vec3 normal;
	vec2 UVCoords;
	mat3 TBN;
} vertexData;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform Camera camera;

uniform Material material;

uniform DirectionalLight directionalLights[10];
uniform int amountOfDirectionalLights;
uniform PointLight pointLights[10];
uniform int amountOfPointLights;
uniform SpotLight spotLights[10];
uniform int amountOfSpotLights;

/* ----------- */
// PROTOTYPES
/* ----------- */

vec3 calculateDirectionalLight(int i, vec3 normalizedNormal);
vec3 calculatePointLight(int i, vec3 normalizedNormal);
vec3 calculateSpotLight(int i, vec3 normalizedNormal);
vec3 calculateCameraLight(vec3 normalizedNormal);

/* ----- */
// MAIN
/* ----- */

void main() {

	// AMBIENT LIGHT (ambientIntensity x (ambientColor || (ambientTextureMap || diffuseTextureMap)))
	// =============================================================================================
	vec3 ambientIntensity = vec3(0.01);
	vec3 ambientLight = ambientIntensity * (material.hasAmbientTextureMap ? texture(material.ambientTextureMapUnit, vertexData.UVCoords).rgb : (material.hasDiffuseTextureMap ? texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : material.ambientColor));

	// NORMAL VECTOR
	// =============
	vec3 normalizedNormal;
	if (material.hasNormalTextureMap) {
		// obtain normal from normal map in range [0,1]
		vec3 normalMapNormal = texture(material.normalTextureMapUnit, vertexData.UVCoords).rgb;
		// transform normal vector to range [-1,1]
		normalizedNormal = normalize(normalMapNormal * 2.0 - 1.0);
		// transform normal vector from tangent space to world space
		normalizedNormal = normalize(vertexData.TBN * normalizedNormal);
	} else {
		normalizedNormal = normalize(vertexData.normal);
	}

	// LIGHT & COLOR
	// =============
	vec3 directionalLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfDirectionalLights; i++)
		directionalLight += calculateDirectionalLight(i, normalizedNormal);

	vec3 pointLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfPointLights; i++)
		pointLight += calculatePointLight(i, normalizedNormal);

	vec3 spotLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < amountOfSpotLights; i++)
		spotLight += calculateSpotLight(i, normalizedNormal);

	vec3 light = ambientLight + directionalLight + pointLight + spotLight + calculateCameraLight(normalizedNormal);
	float alphaChannel = material.hasAlphaTextureMap ? material.alpha * texture(material.alphaTextureMapUnit, vertexData.UVCoords).a : material.alpha;
	if (alphaChannel < 0.1)
		discard;
	color = vec4(light * camera.brightness, alphaChannel);

	// check whether color is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(light, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        brightColor = vec4(light, 1.0);
	else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

vec3 calculateDirectionalLight(int i, vec3 normalizedNormal) {

	// DIFFUSE LIGHT (lightIntensity x (diffuseColor || diffuseTextureMap) x (normal x lightDirection))
	// ================================================================================================
	vec3 normalizedLightDirection = normalize(-directionalLights[i].direction);

	vec3 diffuseLight = directionalLights[i].color * (material.hasDiffuseTextureMap ? texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : material.diffuseColor) * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x (specularColor || specularTextureMap) x (viewDirection x reflectionDirection)^shininess)
	// =========================================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);
	
	// phong model
	//vec3 specularLight = directionalLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// blinn-phong model (shininess needs to be 4 times greater than with phong model)
	vec3 normalizedHalfwayDirection = normalize(normalizedLightDirection + normalizedViewDirection);
	vec3 specularLight = directionalLights[i].color * (material.hasSpecularTextureMap ? vec3(texture(material.specularTextureMapUnit, vertexData.UVCoords).r) : material.specularColor) * pow(max(dot(normalizedNormal, normalizedHalfwayDirection), 0.0), material.shininess * 4.0);

	return diffuseLight + specularLight;
}

vec3 calculatePointLight(int i, vec3 normalizedNormal) {

	// DIFFUSE LIGHT (lightIntensity x (diffuseColor || diffuseTextureMap) x (normal x lightDirection))
	// ================================================================================================
	vec3 normalizedLightDirection = normalize(pointLights[i].position - vertexData.positionWorld);

	vec3 diffuseLight = pointLights[i].color * (material.hasDiffuseTextureMap ? texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : material.diffuseColor) * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x (specularColor || specularTextureMap) x (viewDirection x reflectionDirection)^shininess)
	// =========================================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	// phong model
	//vec3 specularLight = pointLights[i].color * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// blinn-phong model (shininess needs to be 4 times greater than with phong model)
	vec3 normalizedHalfwayDirection = normalize(normalizedLightDirection + normalizedViewDirection);
	vec3 specularLight = pointLights[i].color * (material.hasSpecularTextureMap ? vec3(texture(material.specularTextureMapUnit, vertexData.UVCoords).r) : material.specularColor) * pow(max(dot(normalizedNormal, normalizedHalfwayDirection), 0.0), material.shininess * 4.0);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(pointLights[i].position - vertexData.positionWorld);
	float attenuation = 1.0 / (pointLights[i].attenuation[0] + (pointLights[i].attenuation[1] * distanceToLight) + (pointLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

	return attenuation * (diffuseLight + specularLight);
}

vec3 calculateSpotLight(int i, vec3 normalizedNormal) {
		
	// DIFFUSE LIGHT (lightIntensity x (diffuseColor || diffuseTextureMap) x (normal x lightDirection))
	// ================================================================================================
	vec3 normalizedLightDirection = normalize(spotLights[i].position - vertexData.positionWorld);

	vec3 diffuseLight = spotLights[i].intensity * (material.hasDiffuseTextureMap ? texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : material.diffuseColor) * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x (specularColor || specularTextureMap) x (viewDirection x reflectionDirection)^shininess)
	// =========================================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	// phong model
	//vec3 specularLight = spotLights[i].intensity * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// blinn-phong model (shininess needs to be 4 times greater than with phong model)
	vec3 normalizedHalfwayDirection = normalize(normalizedLightDirection + normalizedViewDirection);
	vec3 specularLight = spotLights[i].intensity * (material.hasSpecularTextureMap ? vec3(texture(material.specularTextureMapUnit, vertexData.UVCoords).r) : material.specularColor) * pow(max(dot(normalizedNormal, normalizedHalfwayDirection), 0.0), material.shininess * 4.0);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(spotLights[i].position - vertexData.positionWorld);
	float attenuation = 1.0 / (spotLights[i].attenuation[0] + (spotLights[i].attenuation[1] * distanceToLight) + (spotLights[i].attenuation[2] * (distanceToLight * distanceToLight)));

	// check if inside light cone and if so calculate a smooth light cone edge
	// =======================================================================
	float theta = dot(normalizedLightDirection, normalize(-spotLights[i].direction));
	float epsilon = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
	float coneIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0); // if outside only black color will be returned

	return coneIntensity * attenuation * (diffuseLight + specularLight);
}

vec3 calculateCameraLight(vec3 normalizedNormal) {
	// DIFFUSE LIGHT (lightIntensity x (diffuseColor || diffuseTextureMap) x (normal x lightDirection))
	// ================================================================================================
	vec3 normalizedLightDirection = normalize(camera.position - vertexData.positionWorld);

	vec3 diffuseLight = camera.intensity * (material.hasDiffuseTextureMap ? texture(material.diffuseTextureMapUnit, vertexData.UVCoords).rgb : material.diffuseColor) * max(dot(normalizedNormal, normalizedLightDirection), 0.0);

	// SPECULAR LIGHT (lightIntensity x (specularColor || specularTextureMap) x (viewDirection x reflectionDirection)^shininess)
	// =========================================================================================================================
	vec3 normalizedViewDirection = normalize(camera.position - vertexData.positionWorld);
	vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedNormal);

	// phong model
	//vec3 specularLight = camera.intensity * material.specularColor * pow(max(dot(normalizedViewDirection, reflectionDirection), 0.0), material.shininess);

	// blinn-phong model (shininess needs to be 4 times greater than with phong model)
	vec3 normalizedHalfwayDirection = normalize(normalizedLightDirection + normalizedViewDirection);
	vec3 specularLight = camera.intensity * (material.hasSpecularTextureMap ? vec3(texture(material.specularTextureMapUnit, vertexData.UVCoords).r) : material.specularColor) * pow(max(dot(normalizedNormal, normalizedHalfwayDirection), 0.0), material.shininess * 4.0);

	// ATTENUATION (1.0 / (constant + (linear * distanceToLight) + (quadratic * (distanceToLight * distanceToLight))))
	// ===============================================================================================================
	float distanceToLight = length(camera.position - vertexData.positionWorld);
	float attenuation = 1.0 / (camera.attenuation[0] + (camera.attenuation[1] * distanceToLight) + (camera.attenuation[2] * (distanceToLight * distanceToLight)));

	// check if inside light cone and if so calculate a smooth light cone edge
	// =======================================================================
	float theta = dot(normalizedLightDirection, normalize(camera.direction));
	float epsilon = camera.innerCutOff - camera.outerCutOff;
	float coneIntensity = clamp((theta - camera.outerCutOff) / epsilon, 0.0, 1.0); // if outside only black color will be returned

	return coneIntensity * attenuation * (diffuseLight + specularLight);
}
