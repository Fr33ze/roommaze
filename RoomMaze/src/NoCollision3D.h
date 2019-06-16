#pragma once
#include "Static3D.h"
class NoCollision3D :
	public Object3D
{
public:
	NoCollision3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	NoCollision3D(const NoCollision3D &o, physx::PxTransform modelMatrix);
	~NoCollision3D();
protected:
	// does not create a shape. never call this
	physx::PxShape* createShape(const char *path) override;
};

