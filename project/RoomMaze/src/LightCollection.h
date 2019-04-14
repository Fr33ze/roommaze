#pragma once

#include <iostream>
#include <sstream>

#include <glm\glm.hpp>
#include <vector>

#include "Shader.h"

class LightCollection {

protected:

	struct DirectionalLight {
		glm::vec3 color;
		glm::vec3 direction;
	};

	struct PointLight {
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 attenuation;
	};

	struct SpotLight {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 intensity;
		float innerAngle;
		float outerAngle;
		glm::vec3 attenuation;
	};

	// shader used for rendering this light collection
	std::shared_ptr<Shader> shader;

	// storage for all lights
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

public:

	/**
	 * CONSTRUCTOR
	 * Creates a light collection.
	 *
	 * @param shader: shader used for rendering this object
	 */
	LightCollection(std::shared_ptr<Shader> shader);

	~LightCollection();

	/**
	 * Adds a directional light to the light collection.
	 *
	 * @param color: color of the light
	 * @param direction: direction of the light
	 */
	void addDirectionalLight(glm::vec3 color, glm::vec3 direction);

	/**
	 * Adds a point light to the light collection.
	 *
	 * @param color: color of the light
	 * @param position: position of the light
	 * @param attenuation: vec3 of floats that stores the attenuation of the light (constant, linear, quadratic)
	 */
	void addPointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation);

	/**
	 * Adds a spot light to the light collection.
	 *
	 * @param position: position of the light
	 * @param direction: direction of the light
	 * @param intensity: intensity (= color) of the light
	 * @param innerAngle: inner angle of the light cone
	 * @param outerAngle: outer angle of the light cone
	 * @param attenuation: vec3 of floats that stores the attenuation of the light (constant, linear, quadratic)
	 */
	void addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 intensity, float innerAngle, float outerAngle, glm::vec3 attenuation);

	// Sets light collection's parameters as uniforms in its specified shader.
	void setUniforms();

};
