#include "ButtonPanel.h"



ButtonPanel::ButtonPanel(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Interactable3D(path, shader, modelMatrix)
{
	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxShape = createShape(path);
	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}

ButtonPanel::ButtonPanel(const ButtonPanel &o, physx::PxTransform modelMatrix)
	: Interactable3D(o, modelMatrix)
{
	pxShape = o.pxShape;

	extern physx::PxPhysics *mPhysics;
	pxActor = mPhysics->createRigidStatic(modelMatrix);

	pxActor->attachShape(*pxShape);
	pxActor->userData = this;

	extern physx::PxScene *pxScene;
	pxScene->addActor(*pxActor);
}


ButtonPanel::~ButtonPanel()
{
}

void ButtonPanel::use(GUI *gui)
{
	if (gui->hasButton() && !buttonplaced) {
		gui->removeButton();
		buttonplaced = true;
		hiddenButton->enable(true);
	}
	else if (buttonplaced && !playpush) {
		animPush->reset();
		playpush = true;
		if (elevatorDoor->isOpen()) {
			elevatorDoor->closeDoor();
		}
	}
}

std::string ButtonPanel::guitext(GUI *gui)
{
	return (gui->hasResistance() ? "Insert Button" : "Find and insert a Button to repair");
}

void ButtonPanel::setHiddenButton(NoCollision3D *hiddenButton)
{
	this->hiddenButton = hiddenButton;
	std::vector<Animation::KeyFrame> keyframesPush = {
		Animation::KeyFrame(0.5, physx::PxVec3(0.f, 0.f, -0.015f), physx::PxVec3(0.f)),
		Animation::KeyFrame(0.5, physx::PxVec3(0.f, 0.f, 0.015f), physx::PxVec3(0.f))
	};
	animPush = new Animation(hiddenButton->getActor(), keyframesPush);
}

void ButtonPanel::setShownButton(NoCollision3D *shownButton)
{
	this->shownButton = shownButton;
}

void ButtonPanel::setElevatorDoor(ElevatorDoor *elevatorDoor)
{
	this->elevatorDoor = elevatorDoor;
}

void ButtonPanel::draw(float dt)
{
	if (!enabled)
		return;

	extern Camera *camera;
	shader->use();

	if (playpush) {
		playpush = animPush->animate(dt);
	}

	shader->setUniform("modelMatrix", pxActor->getGlobalPose());
	camera->setUniforms(shader);
	for (unsigned int i = 0; i < components.size(); i++) {
		components.at(i).draw(shader);
	}

	shader->unuse();
}

void ButtonPanel::pushButton()
{
}

physx::PxShape* ButtonPanel::createShape(const char *path)
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
	temp->setQueryFilterData(physx::PxFilterData(0, INTERACTABLE, 0, 0));
	return temp;
}
