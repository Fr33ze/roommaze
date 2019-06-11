#pragma once

#include "Component3D.h"
#include "Material.h"
#include <vector>

class OBJ
{
public:
	static std::vector<Component3D> read(const char *path, const char *filename);
private:
	static Material readMaterial(const char *path, const char *filename);
};

