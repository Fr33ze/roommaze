#pragma once

#include "Interactable3D.h"
#include "Animation.h"

class ElevatorDoor : public Interactable3D
{
public:
	ElevatorDoor(const char *pathLeft, const char *pathRight, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	ElevatorDoor(const ElevatorDoor &o, physx::PxTransform modelMatrix);
	~ElevatorDoor();
	void use(GUI *gui) override;
	void openDoor();
	void closeDoor();
	std::string guitext(GUI *gui) override;
	void draw(float dt) override;
protected:
	// actor of the right doorwing
	physx::PxRigidActor *pxActorRight;
	// components for right doorwing
	std::vector<Component3D> componentsRight;
	// physx shape for right doorwing
	physx::PxShape *pxShapeLeft;
	// physx shape for right doorwing
	physx::PxShape *pxShapeRight;
	// in this case only creates a sphere query shape (for raycasts)
	physx::PxShape* createShape(const char *path) override;
	// animations for opening the doors
	Animation *animLeft;
	Animation *animRight;
	// should play open/close animation
	bool playopen = false;
	bool playclose = false;
	// current state of doors (true = open, false = closed)
	bool open = false;
};

