#pragma once

#include <map>
#include "Component3D.h"

/*!
 * Wavefront Object (.obj) Reader
 */
class OBJReader {
protected:
	/**
	 * Reads from the specified .mtl file (filename) and returns all materials
	 *
	 * @param path: path of the file without its filename
	 * @param filename: filename of the MTL file without its path
	 */
	static std::unordered_map<std::string, std::shared_ptr<Material>> readMaterials(const char *path, const char *filename);
	/**
	 * Returns the directory path of a full filename
	 *
	 * @param filename: filename of a file
	 */
	static std::string splitFilename(const std::string filename);
public:
	/**
	 * Reads from the specified .obj file (filename) and return a vector of components split up by materials
	 *
	 * @param filename: filename of the OBJ file
	 */
	static std::vector<Component3D> readObject(const char *filename);
};