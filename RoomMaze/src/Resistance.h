#pragma once
#include "Interactable3D.h"
class Resistance : public Interactable3D
{
public:
	Resistance(const char *path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix = glm::mat4(1.0f));
	Resistance(const Resistance &o, glm::mat4 modelMatrix);
	~Resistance();
	void use(GUI::Inventory *inv) override;
	std::string guitext(GUI::Inventory *inv) override;
protected:
	// in this case only creates a sphere query shape (for raycasts)
	void createShape(const char *path) override;
};

