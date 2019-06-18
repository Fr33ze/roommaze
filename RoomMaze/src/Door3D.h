#pragma once
#include "Interactable3D.h"
#include "NoCollision3D.h"
#include "Animation.h"

class Door3D :
	public Interactable3D
{
public:
	Door3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix = physx::PxTransform(physx::PxIdentity));
	Door3D(const Door3D &o, physx::PxTransform modelMatrix);
	~Door3D();
	void use(GUI *gui) override;
	std::string guitext(GUI *gui) override;
	void openDoor();
	void draw(float dt) override;
protected:
	physx::PxShape* createShape(const char *path) override;
	NoCollision3D *animatedKey;
	bool open = false;
	bool playopen = false;
	bool playkey = false;
	Animation *animOpen;
	Animation *animKey;
	ALuint audioSource;
	ALuint lockedBuffer;
	ALuint openBuffer;
	ALuint keyBuffer;
};

