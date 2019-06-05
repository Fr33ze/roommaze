#include "Interactable3D.h"

Interactable3D::Interactable3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Object3D(path, shader, modelMatrix)
{
}

Interactable3D::Interactable3D(const Interactable3D &o, glm::mat4 modelMatrix)
	: Object3D(o, modelMatrix)
{
}

Interactable3D::~Interactable3D()
{
}
