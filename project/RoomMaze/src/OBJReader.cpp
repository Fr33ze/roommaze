#include "OBJReader.h"

std::map<std::string, std::shared_ptr<Material>> OBJReader::ReadMaterials(const char *path, const char *filename) {
	FILE *mtlfile;
	std::string fullpath = std::string(path);
	fullpath += filename;
	errno_t err = fopen_s(&mtlfile, fullpath.c_str(), "r");
	if (err != 0) {
		std::cout << "Unable to open file " << fullpath << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
	//define temporary map
	std::map<std::string, std::shared_ptr<Material>> matMap;
	//define and assign default values for material
	glm::vec3 ka = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 kd = glm::vec3(0.8f, 0.8f, 0.8f);;
	glm::vec3 ks = glm::vec3(1.0f, 1.0f, 1.0f);;
	float alpha = 1.0f;
	float shininess = 0.0f;
	char map_ka[256];
	char map_kd[256];
	char map_ks[256];
	char map_bump[256];
	char map_d[256];
	char mat_name[256];
	map_ka[0] = 0;
	map_kd[0] = 0;
	map_ks[0] = 0;
	map_bump[0] = 0;
	mat_name[0] = 0;

	//start reading .mtl file line by line
	while (1) {
		char lineheader[256];
		int res = fscanf_s(mtlfile, "%s", lineheader, sizeof(lineheader));
		if (res == EOF) {
			//add last material to map
			std::string full_ka = std::string(path);
			if (map_ka[0] == 0)
				full_ka.clear();
			else
				full_ka += map_ka;
			std::string full_kd = std::string(path);
			if (map_kd[0] == 0)
				full_kd.clear();
			else
				full_kd += map_kd;
			std::string full_ks = std::string(path);
			if (map_ks[0] == 0)
				full_ks.clear();
			else
				full_ks += map_ks;
			std::string full_bump = std::string(path);
			if (map_bump[0] == 0)
				full_bump.clear();
			else
				full_bump += map_bump;
			std::string full_d = std::string(path);
			if (map_d[0] == 0)
				full_d.clear();
			else
				full_d += map_d;
			std::shared_ptr<Material> m = std::make_shared<Material>(ka, kd, ks, shininess, alpha, full_kd, full_ks, full_d, full_bump);
			matMap[mat_name] = m;
			break;
		}
		else {
			if (strcmp(lineheader, "Ka") == 0) {
				float k1, k2, k3;
				fscanf_s(mtlfile, "%f %f %f\n", &k1, &k2, &k3);
				ka = glm::vec3(k1, k2, k3);
			}
			else if (strcmp(lineheader, "Kd") == 0) {
				float k1, k2, k3;
				fscanf_s(mtlfile, "%f %f %f\n", &k1, &k2, &k3);
				kd = glm::vec3(k1, k2, k3);
			}
			else if (strcmp(lineheader, "Ks") == 0) {
				float k1, k2, k3;
				fscanf_s(mtlfile, "%f %f %f\n", &k1, &k2, &k3);
				ks = glm::vec3(k1, k2, k3);
			}
			else if (strcmp(lineheader, "Tr") == 0) {
				float tr;
				fscanf_s(mtlfile, "%f\n", &tr);
				alpha = 1 - tr;
			}
			else if (strcmp(lineheader, "d") == 0) {
				fscanf_s(mtlfile, "%f\n", &alpha);
			}
			else if (strcmp(lineheader, "Ns") == 0) {
				fscanf_s(mtlfile, "%f\n", &shininess);
			}
			else if (strcmp(lineheader, "Ns") == 0) {
				fscanf_s(mtlfile, "%f\n", &shininess);
			}
			else if (strcmp(lineheader, "map_Ka") == 0) {
				fscanf_s(mtlfile, "%s\n", &map_ka, 256);
			}
			else if (strcmp(lineheader, "map_Kd") == 0) {
				fscanf_s(mtlfile, "%s\n", &map_kd, 256);
			}
			else if (strcmp(lineheader, "map_Ks") == 0) {
				fscanf_s(mtlfile, "%s\n", &map_ks, 256);
			}
			else if (strcmp(lineheader, "map_Bump") == 0) {
				fscanf_s(mtlfile, "%s\n", &map_bump, 256);
			}
			else if (strcmp(lineheader, "map_d") == 0) {
				fscanf_s(mtlfile, "%s\n", &map_d, 256);
			}
			else if (strcmp(lineheader, "newmtl") == 0) {
				//add material to map unless it is the first iteration and mat_name hasn't been set
				if (mat_name[0] != 0) {
					std::string full_ka = std::string(path);
					if (map_ka[0] == 0)
						full_ka.clear();
					else
						full_ka += map_ka;
					std::string full_kd = std::string(path);
					if (map_kd[0] == 0)
						full_kd.clear();
					else
						full_kd += map_kd;
					std::string full_ks = std::string(path);
					if (map_ks[0] == 0)
						full_ks.clear();
					else
						full_ks += map_ks;
					std::string full_bump = std::string(path);
					if (map_bump[0] == 0)
						full_bump.clear();
					else
						full_bump += map_bump;
					std::string full_d = std::string(path);
					if (map_d[0] == 0)
						full_d.clear();
					else
						full_d += map_d;
					std::shared_ptr<Material> m = std::make_shared<Material>(ka, kd, ks, shininess, alpha, full_kd, full_ks, full_d, full_bump);
					matMap[mat_name] = m;
				}

				//reset coefficients to default
				ka = glm::vec3(0.2f, 0.2f, 0.2f);
				kd = glm::vec3(0.8f, 0.8f, 0.8f);;
				ks = glm::vec3(1.0f, 1.0f, 1.0f);;
				alpha = 1.0f;
				shininess = 0.0f;
				memset(map_ka, 0, sizeof(map_ka));
				memset(map_kd, 0, sizeof(map_kd));
				memset(map_ks, 0, sizeof(map_ks));
				memset(map_bump, 0, sizeof(map_bump));
				memset(map_d, 0, sizeof(map_d));
				memset(mat_name, 0, sizeof(mat_name));

				fscanf_s(mtlfile, "%s\n", &mat_name, 256);
			}
		}
	}
	return matMap;
}

std::string OBJReader::SplitFilename(const std::string filename)
{
	size_t found = filename.find_last_of("/\\");
	return filename.substr(0, found + 1);
}

std::vector<Geometry> OBJReader::ReadObject(const char * filename, std::shared_ptr<Shader> shader)
{
	FILE *objfile;
	errno_t err = fopen_s(&objfile, filename, "r");
	if (err != 0) {
		std::cout << "Unable to open file " << filename << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	//define temporary lists
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::map<std::string, std::shared_ptr<Material>> materialNameMap;
	std::map<std::string, std::vector<std::vector<unsigned int>>> materialFaceMap;
	std::string currentMaterial;
	std::vector<std::vector<unsigned int>> currentFaces = {};

	//start reading .obj file line by line
	while (1) {
		char lineheader[256];
		int res = fscanf_s(objfile, "%s ", lineheader, sizeof(lineheader));
		if (res == EOF) {
			//add last material and its faces to the map (if it doesnt exist yet)
			if (!currentMaterial.empty() && materialFaceMap.count(currentMaterial) == 0) {
				materialFaceMap[currentMaterial] = currentFaces;
			}
			//get last material and add the new faces to its list (if it does already exist)
			else {
				std::vector<std::vector<unsigned int>> &tempfaces = materialFaceMap[currentMaterial];
				tempfaces.insert(tempfaces.end(), currentFaces.begin(), currentFaces.end());
			}
			break;
		}
		else {
			//positions
			if (strcmp(lineheader, "v") == 0) {
				glm::vec3 position;
				fscanf_s(objfile, "%f %f %f\n", &position.x, &position.y, &position.z);
				positions.push_back(position);
			}
			//uv coordinates
			else if (strcmp(lineheader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(objfile, "%f %f\n", &uv.x, &uv.y);

				//Note: OpenGL texture are loaded left to right, bottom to top.
				uv.x = 1 - uv.x;
				uv.y = 1 - uv.y;
				uvs.push_back(uv);
			}
			//normals
			else if (strcmp(lineheader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(objfile, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);
			}
			//temporary faces
			else if (strcmp(lineheader, "f") == 0) {
				std::vector<unsigned int> face;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(objfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					std::cout << "Unable to read file! Try exporting .obj with other settings." << std::endl;
				}
				face.push_back(vertexIndex[0]);
				face.push_back(uvIndex[0]);
				face.push_back(normalIndex[0]);

				face.push_back(vertexIndex[1]);
				face.push_back(uvIndex[1]);
				face.push_back(normalIndex[1]);

				face.push_back(vertexIndex[2]);
				face.push_back(uvIndex[2]);
				face.push_back(normalIndex[2]);

				currentFaces.push_back(face);
			}
			//set current material
			else if (strcmp(lineheader, "usemtl") == 0) {
				char material[256];
				fscanf_s(objfile, "%s\n", &material, 256);

				//skip if its first iteration (currentMaterial is empty in first iteration)
				if (!currentMaterial.empty()) {
					//add current material and its faces to the map (if it doesnt exist yet)
					if (materialFaceMap.count(currentMaterial) == 0) {
						materialFaceMap[currentMaterial] = currentFaces;

						currentFaces = {};
					}
					//get current material and add the new faces to its list (if it does already exist)
					else {
						std::vector<std::vector<unsigned int>> &tempfaces = materialFaceMap[currentMaterial];
						tempfaces.insert(tempfaces.end(), currentFaces.begin(), currentFaces.end());

						currentFaces = {};
					}
				}
				currentMaterial = material;
			}
			//load all materials and its names and insert them into mans
			else if (strcmp(lineheader, "mtllib") == 0) {
				char mtllib[256];
				fscanf_s(objfile, "%s\n", &mtllib, 256);
				materialNameMap = ReadMaterials(SplitFilename(filename).c_str(), mtllib);
			}
		}
	}
	//Geometry split up by materials
	std::vector<Geometry> allGeometries;
	for (std::map<std::string, std::vector<std::vector<unsigned int>>>::iterator iter = materialFaceMap.begin(); iter != materialFaceMap.end(); iter++) {
		//iter->first is material_name
		//iter->second are its faces

		Geometry::GeometryData gd;

		//to reuse already created vertices for face construction
		std::map<std::string, unsigned int> reuseVertexMap;
		unsigned int reuseCount = 0;

		for (std::vector<unsigned int> &face : iter->second) {
			std::string first = std::to_string(face[0]);
			first += face[1];
			first += face[2];
			std::string second = std::to_string(face[3]);
			second += face[4];
			second += face[5];
			std::string third = std::to_string(face[6]);
			third += face[7];
			third += face[8];

			//create vertices only if they do not exist (to save resources)
			//always substract 1 from face[i] because indexing starts at 0 in c++
			if (!reuseVertexMap.count(first)) {
				gd.vertices.push_back(positions[face[0] - 1]);
				gd.UVCoords.push_back(uvs[face[1] - 1]);
				gd.normals.push_back(normals[face[2] - 1]);
				unsigned int index = gd.indices.size() - reuseCount;
				reuseVertexMap[first] = index;
				gd.indices.push_back(index);
			}
			else {
				gd.indices.push_back(reuseVertexMap[first]);
				reuseCount++;
			}

			if (!reuseVertexMap.count(second)) {
				gd.vertices.push_back(positions[face[3] - 1]);
				gd.UVCoords.push_back(uvs[face[4] - 1]);
				gd.normals.push_back(normals[face[5] - 1]);
				unsigned int index = gd.indices.size() - reuseCount;
				reuseVertexMap[second] = index;
				gd.indices.push_back(index);
			}
			else {
				gd.indices.push_back(reuseVertexMap[second]);
				reuseCount++;
			}

			if (!reuseVertexMap.count(third)) {
				gd.vertices.push_back(positions[face[6] - 1]);
				gd.UVCoords.push_back(uvs[face[7] - 1]);
				gd.normals.push_back(normals[face[8] - 1]);
				unsigned int index = gd.indices.size() - reuseCount;
				reuseVertexMap[third] = index;
				gd.indices.push_back(index);
			}
			else {
				gd.indices.push_back(reuseVertexMap[third]);
				reuseCount++;
			}
		}
		allGeometries.push_back(Geometry(gd, glm::mat4(1.0f), shader, materialNameMap[iter->first]));

		//reset GeometryData gd and reuseVertexMap for next material
		gd = {};
		reuseVertexMap.clear();
		reuseCount = 0;
	}

	return allGeometries;
}
