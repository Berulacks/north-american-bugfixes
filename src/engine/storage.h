#pragma once

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef OGL_LD
#define OGL_LD

#include "../../libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

#endif

#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>


//#include <vector>
//#include <map>
//#include <iostream>
//#include <fstream>

#include "../data_structs/model.h"

//Stores actual mesh information
//and handles file IO
class Storage
{


	Assimp::Importer importer;

	public:
		Storage(void);

		static bool readFile(std::string filename, std::string* target);
		static bool checkGLErrors(const char* description="");

		bool storeProgram( Program toAdd );

		//Reads model into memory
		bool readModel(const char* filePath);
		//TODO: figure out how to get a Model class definition in here
		//then have a way to create models, and store them in a 
		//vector.
		//
		Model *loadModel(const char* name);
		bool loadTexture(const char* filePath, const char* name);

		//Create a solid, single colour texture
		//(used for when an object doesn't have
		//a texture)
		GLuint createTexture(glm::vec3 colour);

		Material initMaterial(aiMaterial* material, Program* shader);

		Material getMaterial(const char* name);
		Model getModel(const char* name) { return models[ std::string(name) ]; };

		static std::vector<unsigned int> generateFacesVector(aiFace* assimpFaceArray, int numFaces);
	private:
		std::map<std::string, ModelData> rawModels;
		std::map<std::string, Model> models;
		std::map<std::string, GLuint> textureIDs;
		std::map<std::string, Material> materials;
		std::vector<Program> programs;

		//Eventually this should read text files from the materials
		//folder in order to create actual sets of materials
		void initMaterials(void);



};
