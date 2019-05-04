#pragma once

#include <physx\PxPhysicsAPI.h>

#include "Component3D.h"

class Object3D
{
private:
	// default constructor is private so it cant be used
	Object3D();
protected:
	// actor of the object
	physx::PxRigidActor *pxActor;

	// object consists of multiple components (one for each material)
	std::vector<Component3D> components;

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
	 * COPY CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 * @param modelMatrix: modelMatrix of the new copy
	 */
	Object3D(const Object3D &o, glm::mat4 modelMatrix);
	~Object3D();
	/**
	 * Draws the object and sets the needed Uniforms on the shader
	 *
	 * @param camera: the camera used for rendering
	 */
	void draw(Camera &camera);
	void destroy();
	void addShapesToActor();
};