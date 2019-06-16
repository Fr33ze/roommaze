#include "NoCollision3D.h"


NoCollision3D::NoCollision3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Object3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

NoCollision3D::NoCollision3D(const NoCollision3D & o, physx::PxTransform modelMatrix)
	: Object3D(o, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

NoCollision3D::~NoCollision3D()
{
}

physx::PxShape* NoCollision3D::createShape(const char * path)
{
	return nullptr;
}
