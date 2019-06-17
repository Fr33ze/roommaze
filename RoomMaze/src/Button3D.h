#pragma once

#include "Interactable3D.h"

class Button3D : public Interactable3D
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
	Button3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));

	/**
	 * COPY CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 * @param modelMatrix: modelMatrix of the new copy
	 */
	Button3D(const Button3D &o, physx::PxTransform modelMatrix);
	~Button3D();

	void use(GUI *gui) override;
	std::string guitext(GUI *gui) override;
protected:
	// in this case only creates a sphere query shape (for raycasts)
	physx::PxShape* createShape(const char *path) override;
};