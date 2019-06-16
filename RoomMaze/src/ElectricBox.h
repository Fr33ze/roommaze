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
	void use(GUI::Inventory *inv) override;
	std::string guitext(GUI::Inventory *inv) override;
protected:
	physx::PxShape* createShape(const char *path) override;
	Static3D *hiddenRes;
	Static3D *shownRes;
	ElevatorDoor *elevatorDoor;
};

