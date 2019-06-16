#pragma once
#include "Interactable3D.h"
class Resistance : public Interactable3D
{
public:
	Resistance(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	Resistance(const Resistance &o, physx::PxTransform modelMatrix);
	~Resistance();
	void use(GUI::Inventory *inv) override;
	std::string guitext(GUI::Inventory *inv) override;
protected:
	// in this case only creates a sphere query shape (for raycasts)
	physx::PxShape* createShape(const char *path) override;
};

