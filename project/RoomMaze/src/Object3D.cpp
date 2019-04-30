#include "Object3D.h"
#include "OBJReader.h"


Object3D::Object3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: shader(shader), modelMatrix(modelMatrix)
{
	components = OBJReader::readObject(path);
	//extern physx::PxPhysics *mPhysics;
	//pxActor = mPhysics->createRigidStatic(physx::PxTransform());
}

Object3D::Object3D(const Object3D & o)
{
}

Object3D::~Object3D()
{
}

void Object3D::transform(glm::mat4 transformation)
{
	modelMatrix = transformation * modelMatrix;
}

void Object3D::resetModelMatrix() {
	//modelMatrix = glm::mat4(1.0f);
	//TODO: reset model matrix of actor
}

void Object3D::draw(Camera &camera)
{
	shader->use();
	shader->setUniform("modelMatrix", modelMatrix);
	camera.setUniforms(shader);
	for (unsigned int i = 0; i < components.size(); i++) {
		components.at(i).draw(shader);
	}
	shader->unuse();
}

void Object3D::destroy() {
	for (unsigned int i = 0; i < components.size(); i++) {
		components.at(i).destroy();
	}
}