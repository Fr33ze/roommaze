#pragma once

#include <map>
#include "Geometry.h"

/*!
 * Wavefront Object (.obj) Reader
 */
class OBJReader {
protected:
	/*!
	 * Reads from the specified .mtl file (filename) and returns all materials
	 */
	static std::map<std::string, std::shared_ptr<Material>> ReadMaterials(const char *path, const char *filename);
	/*!
	 * Returns the directory path of a full filename
	 */
	static std::string SplitFilename(const std::string filename);
public:
	/*!
	 * Reads from the specified .obj file (filename) and returns its complete geometry including materials
	 */
	static std::vector<Geometry> ReadObject(const char *filename, std::shared_ptr<Shader> shader);
};