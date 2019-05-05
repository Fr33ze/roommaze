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

	createActor(physx::PxTransform(pose));

	std::vector<std::string> splitfile = OBJReader::splitFilename(path);
	size_t found = splitfile[1].find_last_of(".");
	std::string colfile = splitfile[0] + splitfile[1].substr(0, found + 1) + "col";
	createShape(path);
	pxActor->attachShape(*pxShape);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Dynamic3D::Dynamic3D(const Dynamic3D &o, glm::mat4 modelMatrix)
	: Object3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	createActor(physx::PxTransform(pose));

	pxActor->attachShape(*pxShape);

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Dynamic3D::~Dynamic3D()
{
}

//TODO Does not work yet (maybe should be deleted)
void Dynamic3D::transform(glm::mat4 transformation)
{
	physx::PxMat44 trans = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			trans[x][y] = transformation[x][y];
		}
	}

	extern physx::PxScene *pxScene;
	pxScene->removeActor(*pxActor);

	physx::PxTransform pose = pxActor->getGlobalPose();
	pose.transform(physx::PxTransform(trans));
	pxActor->setGlobalPose(pose);

	pxScene->addActor(*pxActor);
}

//TODO Does not work yet (maybe should be deleted)
void Dynamic3D::resetModelMatrix() {
	pxActor->setGlobalPose(physx::PxTransform(physx::PxIdentity));
}

void Dynamic3D::createShape(const char *path) {
	// physX global variables
	extern physx::PxFoundation *mFoundation;
	extern physx::PxPhysics *mPhysics;
	extern physx::PxPvd *mPvd;
	extern physx::PxCooking *mCooking;

	std::vector<glm::vec3> convexVerts = OBJReader::readCollisionConvex(path);

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(glm::vec3);
	convexDesc.points.data = convexVerts.data();
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	if (!mCooking->cookConvexMesh(convexDesc, buf, &result)) {
		std::cout << "Unable to create convex mesh " << path << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = mPhysics->createConvexMesh(input);

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5), physx::PxReal(0.5), physx::PxReal(0.7));
	pxShape = mPhysics->createShape(physx::PxConvexMeshGeometry(convexMesh), *mat, false, physx::PxShapeFlag::eSIMULATION_SHAPE);
}

void Dynamic3D::createActor(physx::PxTransform transform) {
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidDynamic(transform);
	((physx::PxRigidDynamic*)pxActor)->setMass(0.5f);
}