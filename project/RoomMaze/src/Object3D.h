#pragma once

#include <physx\PxPhysicsAPI.h>

#include "Component3D.h"

class Object3D
{
protected:
	// static actor that never moves
	physx::PxRigidStatic *pxActor;

	// object consists of multiple components (one for each material)
	std::vector<Component3D> components;

	// model matrix of the object
	glm::mat4 modelMatrix;

	// shader used for rendering
	std::shared_ptr<Shader> shader;
public:
	/**
	 * CONSTRUCTOR
	 * Loads the 3D Mesh and Materials and stores them in a vector of component. Also creates a PxActor for this object
	 *
	 * @param path: filename of the OBJ file
	 * @param shader : Shader used to render this object
	 * @param modelMatrix: model matrix of the object
	 */
	Object3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));
	/**
	 * CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 */
	Object3D(const Object3D &o);
	~Object3D();
	/**
	 * Transforms the object (updates the model matrix).
	 *
	 * @param transformation: the transformation matrix to be applied to the object
	 */
	void transform(glm::mat4 transformation);
	// Resets the model matrix to the identity matrix.
	void resetModelMatrix();
	void draw(Camera &camera);
	void destroy();
};