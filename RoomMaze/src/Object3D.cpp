#include "Object3D.h"
#include "OBJReader.h"


Object3D::Object3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: shader(shader)
{
	components = OBJReader::readObject(path);
	render = true;
}

Object3D::Object3D(const Object3D &o, glm::mat4 modelMatrix) {
	components = o.components;
	shader = o.shader;
	render = true;
}

Object3D::~Object3D() {
	extern physx::PxScene *pxScene;
	pxScene->removeActor(*pxActor);
	pxActor->release();
}

void Object3D::draw()
{
	if (!render)
		return;

	extern Camera *camera;
	shader->use();
	shader->setUniform("modelMatrix", pxActor->getGlobalPose());
	camera->setUniforms(shader);
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