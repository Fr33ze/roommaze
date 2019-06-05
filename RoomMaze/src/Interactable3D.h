#pragma once

#include "Object3D.h"

class Interactable3D : public Object3D
{
public:
	// DO NOT USE
	Interactable3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));
	// DO NOT USE
	Interactable3D(const Interactable3D &o, glm::mat4 modelMatrix);

	~Interactable3D();

	/**
	 * Is called when this Object is being used by the player
	 * Needs to be overwritten by the item respectively
	 */
	virtual void use() = 0;
};