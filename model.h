#pragma once
#include "material.h"

struct BufferCombo
{
	const char* name;
	GLuint vao, vertices, indices, normals, texturecoords;

	//REMOVE ME AND UPDATEBCOMBOMAT AND MATERIAL* MATERIAL
	//THESE ARE REPLACED WITH THE MATERIALS VECTOR
	Material mat;
};
//Models are scenes that have their
//corresponding buffers already filled
//out. So, if something has a model, it
//means it's ready to be rendered.
class Model
{
	//The aiScene which
	//contains our actual model data
	const aiScene* scene;
	//Our material object which contains
	//texture data and data for uniforms
	//
	//Technically unnecessary because of
	//the data stored in aiMaterials, but
	//I will eventually get rid of assimp,
	//so necessary until then.
	Material* material;
	//The vertex array object to be loaded
	//on render. This assumes all our buffers
	//are already set up
	//GLuint vao;
	//IDs for buffers. For internal use only,
	//I guess
	std::vector<BufferCombo> bufferIDs;

	public:
		//Materials for each mesh
		std::vector<Material> materials;
		Model(); //this is horrible and you should feel bad
		Model(const aiScene* _scene, Material* mat = NULL);
		const aiScene* getScene(void) { return scene; };
		Material* getMaterial(void);
		BufferCombo getBCombo(int index) { return bufferIDs[index]; };
		unsigned int numBCombos(void) { return bufferIDs.size(); };
		void updateBComboMat(Material mat, int index) { bufferIDs[index].mat = mat; }; 
		GLuint getVAO(int index) { return bufferIDs[index].vao; };
		unsigned int numMeshes(void)  { return bufferIDs.size(); };
		//Create necessary buffer objects
		//and load the actual model data into
		//them.
		void setUpBuffers(void);

	private:

		//Generate indices (convert from ASSIMP faces
		//format to an array of floats, suitable for 
		//OpenGL)
		unsigned int* generateFaces(aiFace* assimpFaceArray, int numFaces);
	



};
