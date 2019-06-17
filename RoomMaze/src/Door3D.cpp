#include "Door3D.h"



Door3D::Door3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	physx::PxTransform keytrans = physx::PxTransform(physx::PxVec3(-0.2f, 1.015f, -0.9f), physx::PxQuat(0.f, 0.f, -0.f, 1.f));

	animatedKey = new NoCollision3D(
		"assets/objects/key.obj",
		shader,
		modelMatrix.transform(keytrans)
	);
	animatedKey->enable(false);

	std::vector<Animation::KeyFrame> keyframesDoor = {
		Animation::KeyFrame(1, physx::PxVec3(0.f), physx::PxVec3(0.f, -110.f, 0.f))
	};
	std::vector<Animation::KeyFrame> keyframesKey = {
		Animation::KeyFrame(0.5, physx::PxVec3(0.15f, 0.f, 0.f), physx::PxVec3(0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.5, physx::PxVec3(-0.15f, 0.f, 0.f), physx::PxVec3(0.f))
	};
	animOpen = new Animation(pxActor, keyframesDoor);
	animKey = new Animation(animatedKey->getActor(), keyframesKey);
	
	pxShape = createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

Door3D::Door3D(const Door3D &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	physx::PxTransform keytrans = physx::PxTransform(physx::PxVec3(-0.2f, 1.015f, -0.9f), physx::PxQuat(0.f, 0.f, -0.f, 1.f));
	animatedKey = new NoCollision3D(*o.animatedKey, modelMatrix.transform(keytrans));
	animatedKey->enable(false);
	std::vector<Animation::KeyFrame> keyframesDoor = {
		Animation::KeyFrame(1, physx::PxVec3(0.f), physx::PxVec3(0.f, -110.f, 0.f))
	};
	std::vector<Animation::KeyFrame> keyframesKey = {
		Animation::KeyFrame(0.5, physx::PxVec3(0.15f, 0.f, 0.f), physx::PxVec3(0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.25, physx::PxVec3(0.f), physx::PxVec3(120.f, 0.f, 0.f)),
		Animation::KeyFrame(0.5, physx::PxVec3(-0.15f, 0.f, 0.f), physx::PxVec3(0.f))
	};
	animOpen = new Animation(pxActor, keyframesDoor);
	animKey = new Animation(animatedKey->getActor(), keyframesKey);

	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}


Door3D::~Door3D()
{
}

void Door3D::use(GUI *gui)
{
	if (gui->hasKey()) {
		openDoor();
	}
}

std::string Door3D::guitext(GUI *gui)
{
	return (open ? "" : "Find and use a key to open");
}

void Door3D::openDoor()
{
	if (!open) {
		animatedKey->enable(true);
		playkey = true;
		open = true;
	}
}

void Door3D::draw(float dt)
{
	if (!enabled)
		return;

	extern Camera *camera;
	shader->use();

	if (playkey) {
		playkey = animKey->animate(dt);
		if (!playkey) {
			playopen = true;
			animatedKey->enable(false);
		}
	}
	if (playopen) {
		playopen = animOpen->animate(dt);
	}

	//door
	shader->setUniform("modelMatrix", pxActor->getGlobalPose());
	camera->setUniforms(shader);
	for (unsigned int i = 0; i < components.size(); i++) {
		components.at(i).draw(shader);
	}
	//key
	animatedKey->draw(dt);

	shader->unuse();
}

physx::PxShape* Door3D::createShape(const char *path)
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