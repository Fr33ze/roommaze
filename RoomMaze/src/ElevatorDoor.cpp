#include "ElevatorDoor.h"

ElevatorDoor::ElevatorDoor(const char *pathLeft, const char *pathRight, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(pathLeft, shader, modelMatrix)
{
	componentsRight = OBJReader::readObject(pathRight);

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);
	pxActorRight = mPhysics->createRigidStatic(modelMatrix);

	std::vector<Animation::KeyFrame> keyframesLeft = {
		Animation::KeyFrame(2.2, physx::PxVec3(0.f, 0.f, .95f), physx::PxVec3(0.f))
	};
	std::vector<Animation::KeyFrame> keyframesRight = {
		Animation::KeyFrame(2.2, physx::PxVec3(0.f, 0.f, -.95f), physx::PxVec3(0.f))
	};
	animLeft = new Animation(pxActor, keyframesLeft);
	animRight = new Animation(pxActorRight, keyframesRight);

	pxShape = createShape(pathLeft);
	pxShapeRight = createShape(pathRight);

	pxActor->attachShape(*pxShape);
	pxActorRight->attachShape(*pxShapeRight);

	pxActor->userData = this;
	pxActorRight->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
	pxScene->addActor(*pxActorRight);

	openBuffer = alutCreateBufferFromFile("assets/audio/elevator_open.wav");
	closeBuffer = alutCreateBufferFromFile("assets/audio/elevator_close.wav");

	alGenSources(1, &audioSource);
	alSource3f(audioSource, AL_POSITION, modelMatrix.p.x, modelMatrix.p.y, modelMatrix.p.z);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 1);
}

ElevatorDoor::ElevatorDoor(const ElevatorDoor &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
}

ElevatorDoor::~ElevatorDoor()
{
}

void ElevatorDoor::draw(float dt) {
	if (!enabled)
		return;

	extern Camera *camera;
	shader->use();

	if (playopen) {
		playopen = animLeft->animate(dt);
		playopen = animRight->animate(dt);
	}
	if (playclose) {
		playclose = animLeft->reverse(dt);
		playclose = animRight->reverse(dt);
	}

	//left door
	shader->setUniform("modelMatrix", pxActor->getGlobalPose());
	camera->setUniforms(shader);
	for (unsigned int i = 0; i < components.size(); i++) {
		components.at(i).draw(shader);
	}
	//right door
	shader->setUniform("modelMatrix", pxActorRight->getGlobalPose());
	for (unsigned int i = 0; i < componentsRight.size(); i++) {
		componentsRight.at(i).draw(shader);
	}

	shader->unuse();
}

bool ElevatorDoor::isOpen()
{
	return open;
}

void ElevatorDoor::use(GUI *gui)
{
}

void ElevatorDoor::openDoor() {
	if (!(playopen || playclose)) {
		animLeft->reset();
		animRight->reset();
		if (!open) {
			playopen = true;
			open = true;
			alSourcei(audioSource, AL_BUFFER, openBuffer);
			alSourcePlay(audioSource);
		}
	}
}

void ElevatorDoor::closeDoor() {
	if (!(playopen || playclose)) {
		animLeft->reset();
		animRight->reset();
		if (open) {
			playclose = true;
			open = false;
			alSourcei(audioSource, AL_BUFFER, closeBuffer);
			alSourcePlay(audioSource);
		}
	}
}

std::string ElevatorDoor::guitext(GUI *gui)
{
	return "Repair the switch box to your right.";
}

physx::PxShape* ElevatorDoor::createShape(const char *path)
{
	extern physx::PxPhysics *mPhysics;
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
		return nullptr;
	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	physx::PxTriangleMeshGeometry trigeom = physx::PxTriangleMeshGeometry(mPhysics->createTriangleMesh(readBuffer));
	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	physx::PxShape *temp = mPhysics->createShape(trigeom, *mat, false);
	temp->setQueryFilterData(physx::PxFilterData(COLLISION, INTERACTABLE, 0, 0));
	return temp;
}
