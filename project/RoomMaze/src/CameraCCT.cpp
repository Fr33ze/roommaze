#include "Camera.h"

void Camera::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	//TODO action to perform on shape hit
}

physx::PxControllerBehaviorFlags Camera::getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor)
{
	return physx::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

physx::PxControllerBehaviorFlags Camera::getBehaviorFlags(const physx::PxController&)
{
	return physx::PxControllerBehaviorFlags(0);
}

physx::PxControllerBehaviorFlags Camera::getBehaviorFlags(const physx::PxObstacle&)
{
	return physx::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | physx::PxControllerBehaviorFlag::eCCT_SLIDE;
}

physx::PxQueryHitType::Enum Camera::preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags)
{
	return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum Camera::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit)
{
	return physx::PxQueryHitType::eBLOCK;
}