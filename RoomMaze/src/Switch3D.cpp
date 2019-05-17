#include "Switch3D.h"

Switch3D::Switch3D(const char * pathCover, const char * pathLever, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix)
	: Object3D(pathCover, shader, modelMatrix)
{

}

void Switch3D::draw(Camera & camera)
{
}
