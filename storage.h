#include <vector>
#include <map>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#include <iostream>
#include <fstream>
#include "program.h"
#include "materials/material.h"


class Storage
{

	public:
		Storage(void);

		static bool readFile(std::string filename, std::string* target);
		static bool checkGLErrors(const char* description="");

		bool loadModel(const char* filePath);
		bool loadTexture(const char* filePath, const char* name);
	private:
		std::vector<aiScene> rawModels;
		std::vector<const char*> textures;
		std::map<const char*, int> textureIDs;
		std::map<const char*, Material> materials;



};
