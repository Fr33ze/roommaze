#include "Battery.h"

Battery::Battery(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(physx::PxTransform(pose));

	createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Battery::Battery(const Battery &o, glm::mat4 modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(physx::PxTransform(pose));

	pxActor->attachShape(*pxShape);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Battery::~Battery()
{
}

void Battery::createShape(const char *path) {
	// physX global variables
	extern physx::PxFoundation *mFoundation;
	extern physx::PxPhysics *mPhysics;
	extern physx::PxPvd *mPvd;
	extern physx::PxCooking *mCooking;

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.3f));

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	pxShape = mPhysics->createShape(geom, *mat, false);
	pxShape->setQueryFilterData(physx::PxFilterData(INTERACTABLE, 0, 0, 0));
}

void Battery::use() {
	//TODO use code here
	std::cout << "hello" << std::endl;
}