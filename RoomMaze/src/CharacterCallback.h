#pragma once

#include <physx\PxPhysicsAPI.h>

class CharacterCallback : public physx::PxUserControllerHitReport, public physx::PxControllerBehaviorCallback, public physx::PxQueryFilterCallback {
	// Implements PxUserControllerHitReport
	virtual void onShapeHit(const physx::PxControllerShapeHit& hit);
	virtual void onControllerHit(const physx::PxControllersHit& hit) {}
	virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) {}

	// Implements PxControllerBehaviorCallback
	virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor);
	virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxController& controller);
	virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxObstacle& obstacle);

	// Implements PxQueryFilterCallback
	virtual physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags);
	virtual	physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit);
};