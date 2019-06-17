#pragma once
#include "Interactable3D.h"
#include "NoCollision3D.h"
#include "ElevatorDoor.h"

class ButtonPanel :
	public Interactable3D
{
public:
	ButtonPanel(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	ButtonPanel(const ButtonPanel &o, physx::PxTransform modelMatrix);
	~ButtonPanel();
	void use(GUI *gui) override;
	std::string guitext(GUI *gui) override;
	void setHiddenButton(NoCollision3D *hiddenButton);
	void setShownButton(NoCollision3D *shownButton);
	void setElevatorDoor(ElevatorDoor *elevatorDoor);
	void draw(float dt) override;
	void pushButton();
protected:
	physx::PxShape* createShape(const char *path) override;
	// button without collision that is activated in use()
	NoCollision3D *hiddenButton;
	// button without collision is shown from the beginning
	NoCollision3D *shownButton;
	ElevatorDoor *elevatorDoor;
	bool playpush = false;
	bool buttonplaced = false;
	Animation *animPush;
};