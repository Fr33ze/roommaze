#include "Button3D.h"

Button3D::Button3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
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

Button3D::Button3D(const Button3D &o, physx::PxTransform modelMatrix)
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

Button3D::~Button3D()
{
}

physx::PxShape* Button3D::createShape(const char *path) {
	extern physx::PxPhysics *mPhysics;

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.3f));

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	physx::PxShape *temp = mPhysics->createShape(geom, *mat, false, physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	temp->setQueryFilterData(physx::PxFilterData(0, INTERACTABLE, 0, 0));
	return temp;
}

void Button3D::use(GUI *gui) {
	if (!enabled)
		return;

	if (particles) {
		physx::PxVec3 pos = pxActor->getGlobalPose().p;
		particles->setOrigin(glm::vec3(pos.x, pos.y, pos.z));
		particles->enableFor(0.5f);
	}

	gui->addButton();
	pxActor->detachShape(*pxShape);
	enabled = false;
}

std::string Button3D::guitext(GUI *gui) {
	return (enabled ? "Take Elevator button" : "");
}