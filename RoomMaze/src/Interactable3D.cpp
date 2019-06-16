#include "Interactable3D.h"

Interactable3D::Interactable3D(const char *path, std::shared_ptr<Shader> shader, physx::PxTransform modelMatrix)
	: Object3D(path, shader, modelMatrix)
{
}

Interactable3D::Interactable3D(const Interactable3D &o, physx::PxTransform modelMatrix)
	: Object3D(o, modelMatrix)
{
}

Interactable3D::~Interactable3D()
{
}