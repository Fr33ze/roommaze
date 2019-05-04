#include "Static3D.h"
#include "OBJReader.h"


Static3D::Static3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Object3D(path, shader, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(physx::PxTransform(pose));
	addShapesToActor();

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Static3D::Static3D(const Object3D &o, glm::mat4 modelMatrix)
	: Object3D(o, modelMatrix)
{
	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(physx::PxTransform(pose));
	addShapesToActor();
	
	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Static3D::~Static3D()
{
}