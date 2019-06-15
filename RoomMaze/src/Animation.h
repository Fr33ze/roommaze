#pragma once

#include <physx\PxPhysicsAPI.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Animation
{
public:
	struct KeyFrame {
		float time;
		physx::PxVec3 translate;
		physx::PxVec3 rotate;

		KeyFrame(float time, physx::PxVec3 translate, physx::PxVec3 rotate)
			: time(time), translate(translate), rotate(rotate) {}
		~KeyFrame() {}
	};

	Animation(physx::PxRigidActor *actor, std::vector<KeyFrame> keyframes);
	~Animation();
	// returns true if animation goes on, false if animation stops
	bool animate(float dt);
	// same as animate() but backwards
	bool reverse(float dt);
	void reset();
private:
	float progresstime;
	unsigned int stage;
	unsigned int laststage;
	physx::PxRigidActor *actor;
	std::vector<KeyFrame> keyframes;
};

