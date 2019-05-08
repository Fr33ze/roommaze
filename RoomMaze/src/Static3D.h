#pragma once

#include <physx\PxPhysicsAPI.h>

#include "Component3D.h"
#include "Object3D.h"

class Static3D : public Object3D
{
public:
	/**
	 * CONSTRUCTOR
	 * Loads the 3D Mesh and Materials and stores them in a vector of component. Also creates a PxActor for this object
	 *
	 * @param path: filename of the OBJ file
	 * @param shader : Shader used to render this object
	 * @param modelMatrix: model matrix of the object
	 */
	Static3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));
	/**
	 * COPY CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 * @param modelMatrix: modelMatrix of the new copy
	 */
	Static3D(const Static3D &o, glm::mat4 modelMatrix);
	~Static3D();

protected:
	void createShape(const char *path) override;
};