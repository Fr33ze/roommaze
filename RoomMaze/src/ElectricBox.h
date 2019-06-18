#pragma once
#include "Interactable3D.h"
#include "NoCollision3D.h"
#include "ElevatorDoor.h"

class ElectricBox :
	public Interactable3D
{
public:
	ElectricBox(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	ElectricBox(const ElectricBox &o, physx::PxTransform modelMatrix);
	~ElectricBox();
	void use(GUI *gui) override;
	std::string guitext(GUI *gui) override;
	void setHiddenRes(NoCollision3D *hiddenRes);
	void setShownRes(NoCollision3D *shownRes);
	void setElevatorDoor(ElevatorDoor *elevatorDoor);
protected:
	physx::PxShape* createShape(const char *path) override;
	// resistance without collision that is activated in use()
	NoCollision3D *hiddenRes;
	// resistance without collision that is shown from the beginning
	NoCollision3D *shownRes;
	ElevatorDoor *elevatorDoor;
	ALuint audioSource;
	ALuint lightningBuffer;
};

