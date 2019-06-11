#include "ElevatorDoor.h"

ElevatorDoor::ElevatorDoor(const char *pathLeft, const char *pathRight, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Interactable3D(pathLeft, shader, modelMatrix)
{
	componentsRight = OBJReader::readObject(pathRight);

	physx::PxMat44 pose = physx::PxMat44();
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			pose[x][y] = modelMatrix[x][y];
		}
	}

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidDynamic(physx::PxTransform(pose));
	pxActorRight = mPhysics->createRigidDynamic(physx::PxTransform(pose));

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.3f));
	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	pxShape = mPhysics->createShape(geom, *mat, false);
	pxShape->setQueryFilterData(physx::PxFilterData(INTERACTABLE, 0, 0, 0));

	std::vector<std::string> splitfile = OBJReader::splitFilename(pathLeft);
	size_t found = splitfile[1].find_last_of(".");
	std::string colfile = splitfile[0] + splitfile[1].substr(0, found + 1) + "col";
	pxShapeLeft = createShape(colfile.c_str());
	splitfile = OBJReader::splitFilename(pathRight);
	found = splitfile[1].find_last_of(".");
	colfile = splitfile[0] + splitfile[1].substr(0, found + 1) + "col";
	pxShapeRight = createShape(colfile.c_str());

	pxActor->attachShape(*pxShape);
	pxActor->attachShape(*pxShapeLeft);
	pxActorRight->attachShape(*pxShapeRight);

	pxActor->userData = this;
	pxActorRight->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

ElevatorDoor::ElevatorDoor(const ElevatorDoor & o, glm::mat4 modelMatrix)
	: Interactable3D(o, modelMatrix)
{
}

ElevatorDoor::~ElevatorDoor()
{
}

void ElevatorDoor::use(GUI::Inventory * inv)
{
	pxActor->detachShape(*pxShape);
}

std::string ElevatorDoor::guitext(GUI::Inventory * inv)
{
	return "Repiar the electro box to your right.";
}

physx::PxShape* ElevatorDoor::createShape(const char *path)
{
	extern physx::PxPhysics *mPhysics;
	extern physx::PxCooking *mCooking;

	std::vector<glm::vec3> convexVerts = OBJReader::readCollisionConvex(path);

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = convexVerts.size();
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

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.7f));
	return mPhysics->createShape(physx::PxConvexMeshGeometry(convexMesh), *mat, false);
}
