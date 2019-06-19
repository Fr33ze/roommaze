#include "Resistance.h"



Resistance::Resistance(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxShape = createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);

	takeBuffer = alutCreateBufferFromFile("assets/audio/item_pickup.wav");

	alGenSources(1, &audioSource);
	alSource3f(audioSource, AL_POSITION, modelMatrix.p.x, modelMatrix.p.y, modelMatrix.p.z);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 1);
	alSourcei(audioSource, AL_BUFFER, takeBuffer);
}

Resistance::Resistance(const Resistance &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);

	takeBuffer = alutCreateBufferFromFile("assets/audio/item_pickup.wav");

	alGenSources(1, &audioSource);
	alSource3f(audioSource, AL_POSITION, modelMatrix.p.x, modelMatrix.p.y, modelMatrix.p.z);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 1);
	alSourcei(audioSource, AL_BUFFER, takeBuffer);
}

Resistance::~Resistance()
{
}

physx::PxShape* Resistance::createShape(const char *path) {
	// physX global variables
	extern physx::PxFoundation *mFoundation;
	extern physx::PxPhysics *mPhysics;
	extern physx::PxPvd *mPvd;
	extern physx::PxCooking *mCooking;

	physx::PxSphereGeometry geom = physx::PxSphereGeometry(physx::PxReal(0.3f));

	physx::PxMaterial *mat = mPhysics->createMaterial(physx::PxReal(0.5f), physx::PxReal(0.5f), physx::PxReal(0.6f));
	physx::PxShape *temp = mPhysics->createShape(geom, *mat, false, physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	temp->setQueryFilterData(physx::PxFilterData(0, INTERACTABLE, 0, 0));
	return temp;
}

void Resistance::use(GUI *gui) {
	if (!enabled)
		return;

	if (particles) {
		physx::PxVec3 pos = pxActor->getGlobalPose().p;
		particles->setOrigin(glm::vec3(pos.x, pos.y, pos.z));
		particles->enableFor(0.5f);
	}

	alSourcePlay(audioSource);
	gui->addResistance();
	pxActor->detachShape(*pxShape);
	enabled = false;
}

std::string Resistance::guitext(GUI *gui) {
	return (enabled ? "Pick up Resistance" : "");
}