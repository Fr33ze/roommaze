#include "ElectricBox.h"



ElectricBox::ElectricBox(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxShape = createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);

	lightningBuffer = alutCreateBufferFromFile("assets/audio/electric_box_lightning.wav");

	alGenSources(1, &audioSource);
	alSource3f(audioSource, AL_POSITION, modelMatrix.p.x, modelMatrix.p.y, modelMatrix.p.z);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 0.5);
	alSourcei(audioSource, AL_LOOPING, AL_TRUE);
	alSourcei(audioSource, AL_BUFFER, lightningBuffer);
}

ElectricBox::ElectricBox(const ElectricBox &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);

	lightningBuffer = alutCreateBufferFromFile("assets/audio/electric_box_lightning.wav");

	alGenSources(1, &audioSource);
	alSource3f(audioSource, AL_POSITION, modelMatrix.p.x, modelMatrix.p.y, modelMatrix.p.z);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 0.5);
	alSourcei(audioSource, AL_LOOPING, AL_TRUE);
	alSourcei(audioSource, AL_BUFFER, lightningBuffer);
	alSourcePlay(audioSource);
}


ElectricBox::~ElectricBox()
{
}

void ElectricBox::use(GUI *gui)
{
	if (gui->hasResistance() && elevatorDoor && hiddenRes) {
		gui->removeResistance();
		hiddenRes->enable(true);
		elevatorDoor->openDoor();

		gui->removeResistance();

		alSourcePause(audioSource);

		extern Camera *camera;
		camera->turnElectricBoxOff();

		if (particles) {
			particles->disable();
		}
	}
}

std::string ElectricBox::guitext(GUI *gui)
{
	if (gui->hasResistance()) {
		return "Insert Resistor";
	}
	else if (elevatorDoor->isOpen()) {
		return "Oh, the elevator is provided with electricity again!";
	}
	else {
		return "This switch box serves as power supply. The missing resistor is causing a shortage.";
	}
}

void ElectricBox::setHiddenRes(NoCollision3D *hiddenRes)
{
	this->hiddenRes = hiddenRes;
}

void ElectricBox::setShownRes(NoCollision3D *shownRes)
{
	this->shownRes = shownRes;
}

void ElectricBox::setElevatorDoor(ElevatorDoor *elevatorDoor)
{
	this->elevatorDoor = elevatorDoor;
}

void ElectricBox::startSound()
{
	alSourcePlay(audioSource);
}

physx::PxShape* ElectricBox::createShape(const char *path)
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
