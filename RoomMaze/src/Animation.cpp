#include "Animation.h"



Animation::Animation(physx::PxRigidActor *actor, std::vector<KeyFrame> keyframes)
	: actor(actor), keyframes(keyframes)
{
	laststage = keyframes.size() - 1;
	stage = 0;
	progresstime = 0.f;
}


Animation::~Animation()
{
}

void Animation::reset() {
	stage = 0;
	progresstime = 0.f;
}

bool Animation::animate(float dt)
{
	if (dt > keyframes[stage].time - progresstime) {
		dt -= (keyframes[stage].time - progresstime);
		if (!animate(keyframes[stage].time - progresstime)) {
			return false;
		}
	}

	float progress = dt / keyframes[stage].time;
	float degrees = keyframes[stage].rotate.maxElement() * progress;
	physx::PxVec3 v = keyframes[stage].rotate / degrees;
	physx::PxTransform transformation = physx::PxTransform(
		keyframes[stage].translate * progress,
		physx::PxQuat(glm::radians(degrees), keyframes[stage].rotate.getNormalized())
	);
	actor->setGlobalPose(actor->getGlobalPose().transform(transformation));

	progresstime += dt;
	if (progresstime >= keyframes[stage].time) {
		progresstime = 0;
		stage++;
		if (stage >= keyframes.size()) {
			return false;
		}
	}
	return true;
}

bool Animation::reverse(float dt) {
	if (dt > keyframes[laststage - stage].time - progresstime) {
		dt -= (keyframes[laststage - stage].time - progresstime);
		if (!reverse(keyframes[laststage - stage].time - progresstime)) {
			return false;
		}
	}
	//-dt to make progress negative (transform backwards)
	float progress = -dt / keyframes[laststage - stage].time;
	float radians = 0;// glm::radians(keyframes[laststage - stage].rotate.maxElement() * progress);
	physx::PxTransform transformation = physx::PxTransform(
		keyframes[laststage - stage].translate * progress,
		physx::PxQuat(radians, keyframes[laststage - stage].rotate.getNormalized())
	);
	actor->setGlobalPose(actor->getGlobalPose().transform(transformation));

	progresstime += dt;
	if (progresstime >= keyframes[laststage - stage].time) {
		progresstime = 0;
		stage++;
		if (stage >= keyframes.size()) {
			return false;
		}
	}
	return true;
}