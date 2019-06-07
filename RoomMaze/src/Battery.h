#pragma once

#include "Interactable3D.h"

class Battery : public Interactable3D
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
	Battery(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));

	/**
	 * COPY CONSTRUCTOR
	 * Duplicates the object. Reassigns its PxActor with a new instance of PxActor.
	 *
	 * @param o: object to copy
	 * @param modelMatrix: modelMatrix of the new copy
	 */
	Battery(const Battery &o, glm::mat4 modelMatrix);
	~Battery();

	void use(GUI::Inventory *inv) override;
	std::string guitext() override;
protected:
	// in this case only creates a sphere query shape (for raycasts)
	void createShape(const char *path) override;
};