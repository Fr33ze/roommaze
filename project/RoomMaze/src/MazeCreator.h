#pragma once

#include <unordered_map>
#include <vector>

#include "Geometry.h"

// stores all objects
enum Object {
	WALL_DIMENSION_1,
	WALL_DIMENSION_2,
	WALL_DIMENSION_3,
	STAIRCASE,
	FLOOR
};

class MazeCreator {

protected:

	// an unordnered map that stores all data that is necessary to create a maze
	std::unordered_map<Object, std::vector<Geometry>> objects;

public:

	/**
	 * CONSTRUCTOR
	 * Creates a maze with the given objects.
	 *
	 * @param objects: an unordnered map that stores all data that is necessary to create a maze
	 */
	MazeCreator(std::unordered_map<Object, std::vector<Geometry>> objects);

	MazeCreator();

	~MazeCreator();

};
