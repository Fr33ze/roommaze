#include "Key3D.h"

Key3D::Key3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
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

Key3D::Key3D(const Key3D &o, physx::PxTransform modelMatrix)
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

Key3D::~Key3D()
{
}

physx::PxShape* Key3D::createShape(const char *path) {
	extern physx::PxPhysics *mPhysics;

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.7f));

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	physx::PxShape *temp = mPhysics->createShape(geom, *mat, false, physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	temp->setQueryFilterData(physx::PxFilterData(0, INTERACTABLE, 0, 0));
	return temp;
}

void Key3D::use(GUI::Inventory *inv) {
	if (!enabled)
		return;
	inv->key = true;
	pxActor->detachShape(*pxShape);
	enabled = false;
}

std::string Key3D::guitext(GUI::Inventory *inv) {
	return (enabled ? "Take Key" : "");
}