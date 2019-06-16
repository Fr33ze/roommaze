#pragma once

#include "Object3D.h"
#include "GUI.h"

class Interactable3D : public Object3D
{
public:
	// DO NOT USE
	Interactable3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	// DO NOT USE
	Interactable3D(const Interactable3D &o, physx::PxTransform modelMatrix);

	~Interactable3D();

	/**
	 * Is called when this Object is being used by the player
	 * Needs to be overwritten by the item respectively
	 *
	 * @param inv: inventory of the player
	 * @return type of the Interactable
	 */
	virtual void use(GUI *gui) = 0;

	/**
	 * @param inv: inventory of the player
	 * @return text shown on the gui when focusing this object
	 */
	virtual std::string guitext(GUI *gui) = 0;
};