#pragma once

#include "Dynamic3D.h"

class Switch3D : public Dynamic3D
{
public:
	/**
	 * CONSTRUCTOR
	 * Loads the 3D Mesh and Materials and stores them in a vector of component. Also creates a PxActor for this object
	 *
	 * @param pathCover: filename of the cover OBJ file
	 * @param pathLever: filename of the lever OBJ file
	 * @param shader : Shader used to render this object
	 * @param modelMatrix: model matrix of the object
	 */
	Switch3D(const char *pathCover, const char *pathLever, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));

	/**
	 * COPY CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 * @param modelMatrix: modelMatrix of the new copy
	 */
	Switch3D(const Switch3D &o, glm::mat4 modelMatrix);
	~Switch3D();
protected:
	// actor that controls the lever of the switch
	physx::PxRigidDynamic *leverActor;
};