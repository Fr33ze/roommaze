#pragma once
#include "Object3D.h"
class Water3D :
	public Object3D
{
public:
	Water3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	Water3D(const Water3D &o, physx::PxTransform modelMatrix);
	~Water3D();
protected:
	physx::PxShape* createShape(const char *path) override;
};

