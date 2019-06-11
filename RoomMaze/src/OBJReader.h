#pragma once

#include <robinhood\robin_hood.h>
#include "Component3D.h"

/*!
 * Wavefront Object (.obj) Reader
 */
class OBJReader {
private:
	/**
	 * Reads from the specified .mtl file (filename) and returns all materials
	 *
	 * @param path: path of the file without its filename
	 * @param filename: filename of the MTL file without its path
	 */
	static robin_hood::unordered_flat_map<std::string, std::shared_ptr<Material>> readMaterials(const char *path, const char *filename);
public:
	/**
	 * Returns the directory path and the filename of a full filename
	 *
	 * @param filename: filename of a file
	 */
	static std::vector<std::string> splitFilename(const std::string filename);
	/**
	 * Reads from the specified .obj file (filename) and return a vector of components split up by materials
	 *
	 * @param filename: filename of the OBJ file
	 */
	static std::vector<Component3D> readObject(const char *filename);
	/**
	 * Reads from the specified .col file (filename) and returns its collision vertices
	 *
	 * @param filename: filename of the COL file
	 */
	static std::vector<glm::vec3> readCollisionConvex(const char *filename);
	/**
	 * Reads from the specified .obj file (filename) and returns its vertices and faces
	 *
	 * @param filename: filename of the OBJ file
	 */
	static Component3D::GeometryData readCollisionTrimesh(const char *filename);
};