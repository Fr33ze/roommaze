#pragma once

#include "Interactable3D.h"
#include "Animation.h"

class ElevatorDoor : public Interactable3D
{
public:
	ElevatorDoor(const char *pathLeft, const char *pathRight, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));
	ElevatorDoor(const ElevatorDoor &o, glm::mat4 modelMatrix);
	~ElevatorDoor();
	void use(GUI::Inventory *inv) override;
	std::string guitext(GUI::Inventory *inv) override;
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
	Animation *openLeft;
	Animation *openRight;
	bool playopenleft = false;
	bool playopenright = false;
	bool playcloseleft = false;
	bool playcloseright = false;
};

