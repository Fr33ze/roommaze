#pragma once
#include "Interactable3D.h"
#include "Static3D.h"
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
	void setHiddenRes(Static3D *hiddenRes);
	void setShownRes(Static3D *shownRes);
	void setElevatorDoor(ElevatorDoor *elevatorDoor);
protected:
	physx::PxShape* createShape(const char *path) override;
	// resistance as Static3D that is activated in use()
	Static3D *hiddenRes;
	// resistance as Static3D that is shown from the beginning
	Static3D *shownRes;
	ElevatorDoor *elevatorDoor;
};

