#include "Battery.h"

Battery::Battery(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxShape = createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Battery::Battery(const Battery &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Battery::~Battery()
{
}

physx::PxShape* Battery::createShape(const char *path) {
	extern physx::PxPhysics *mPhysics;

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.3f));

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	physx::PxShape *temp = mPhysics->createShape(geom, *mat, false, physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	temp->setQueryFilterData(physx::PxFilterData(0, INTERACTABLE, 0, 0));
	return temp;
}

void Battery::use(GUI::Inventory *inv) {
	if (!enabled)
		return;
	inv->batteries++;
	pxActor->detachShape(*pxShape);
	enabled = false;
}

std::string Battery::guitext(GUI::Inventory *inv) {
	return (enabled ? "Take Battery" : "");
}