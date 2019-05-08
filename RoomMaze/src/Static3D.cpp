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

	createShape(path);
	pxActor->attachShape(*pxShape);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Static3D::Static3D(const Static3D &o, glm::mat4 modelMatrix)
	: Object3D(o, modelMatrix)
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

	//TODO richtig so?
	pxActor->attachShape(*pxShape);
	
	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Static3D::~Static3D()
{
}

void Static3D::createShape(const char *path) {
	// physX global variables
	extern physx::PxFoundation *mFoundation;
	extern physx::PxPhysics *mPhysics;
	extern physx::PxPvd *mPvd;
	extern physx::PxCooking *mCooking;

	Component3D::GeometryData gd = OBJReader::readCollisionTrimesh(path);

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = gd.vertices.size();
	meshDesc.points.stride = sizeof(glm::vec3);
	meshDesc.points.data = gd.vertices.data();
	meshDesc.triangles.count = gd.indices.size() / 3;
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.data = gd.indices.data();
	physx::PxDefaultMemoryOutputStream writeBuffer;
	physx::PxTriangleMeshCookingResult::Enum result;
	bool status = mCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return;
	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	physx::PxTriangleMeshGeometry trigeom = physx::PxTriangleMeshGeometry(mPhysics->createTriangleMesh(readBuffer));
	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	pxShape = mPhysics->createShape(trigeom, *mat, false, physx::PxShapeFlag::eSIMULATION_SHAPE);
}