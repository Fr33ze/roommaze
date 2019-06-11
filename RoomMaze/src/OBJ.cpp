#include "OBJ.h"

std::vector<Component3D> OBJ::read(const char * path, const char * filename)
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
	Material *currentMtl;
	char mtllib[256];

	while (1) {
		char lineheader[256];
		int res = fscanf_s(objfile, "%s ", lineheader, sizeof(lineheader));
		if (res == EOF) {
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

				//Note: OpenGL texture are loaded left to right, bottom to top. (INACTIVE)
				//Note: Blender exports UVs mirrored on the y axis (ACTIVE)
				//uv.x = 1 - uv.x;
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

				//currentFaces.push_back(face);
			}
			//set current material
			else if (strcmp(lineheader, "usemtl") == 0) {
				char material[256];
				fscanf_s(objfile, "%s\n", &material, 256);

			}
			//load all materials and its names and insert them into mans
			else if (strcmp(lineheader, "mtllib") == 0) {
				fscanf_s(objfile, "%s\n", &mtllib, 256);
			}
		}
	}

	return std::vector<Component3D>();
}


