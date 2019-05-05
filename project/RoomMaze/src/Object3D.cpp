#include "Object3D.h"
#include "OBJReader.h"


Object3D::Object3D(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: shader(shader)
{
	components = OBJReader::readObject(path);
}

Object3D::Object3D(const Object3D &o, glm::mat4 modelMatrix) {
	components = o.components;
	shader = o.shader;
}

Object3D::~Object3D() {
	extern physx::PxScene *pxScene;
	pxScene->removeActor(*pxActor);
	pxActor->release();
}

void Object3D::draw(Camera &camera)
{
	//std::cout << "x: " << pxActor->getGlobalPose().p.x << " y: " << pxActor->getGlobalPose().p.y << " z: " << pxActor->getGlobalPose().p.z << std::endl;
	//getchar();
	shader->use();
	shader->setUniform("modelMatrix", pxActor->getGlobalPose());
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