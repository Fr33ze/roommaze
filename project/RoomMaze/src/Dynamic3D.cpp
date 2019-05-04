#include "Dynamic3D.h"
#include "OBJReader.h"


Dynamic3D::Dynamic3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Object3D(path, shader, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidDynamic(physx::PxTransform(pose));
	//((physx::PxRigidDynamic*)pxActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	addShapesToActor();

	//TODO Change velocity
	//((physx::PxRigidDynamic*)pxActor)->setLinearVelocity(physx::PxVec3(0.0f, -0.5f, 0.0f));

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Dynamic3D::Dynamic3D(const Object3D &o, glm::mat4 modelMatrix)
	: Object3D(o, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidDynamic(physx::PxTransform(pose));
	//((physx::PxRigidDynamic*)pxActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	addShapesToActor();

	//TODO Change velocity
	//((physx::PxRigidDynamic*)pxActor)->setLinearVelocity(physx::PxVec3(0.0f, -0.5f, 0.0f));

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Dynamic3D::~Dynamic3D()
{
}

void Dynamic3D::transform(glm::mat4 transformation)
{
	physx::PxMat44 trans = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			trans[x][y] = transformation[x][y];
		}
	}
	physx::PxTransform pose = pxActor->getGlobalPose();
	pose.transform(physx::PxTransform(trans));
	((physx::PxRigidDynamic*)pxActor)->setGlobalPose(pose);
}

void Dynamic3D::resetModelMatrix() {
	pxActor->setGlobalPose(physx::PxTransform(physx::PxIdentity));
}