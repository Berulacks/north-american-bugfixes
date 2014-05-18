#include "storage.h"

struct BufferCombo
{
	const char* name;
	GLuint vao, vertices, indices, normals, texturecoords;
};
//Models are scenes that have their
//corresponding buffers already filled
//out. So, if something has a model, it
//means it's ready to be rendered.
class Model
{
	//The aiScene which
	//contains our actual model data
	aiScene* scene;
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
		Model();
		//GLuint getVao(void) { return vao; };

	private:
		//Create necessary buffer objects
		//and load the actual model data into
		//them.
		void setUpBuffers(void);

		//Generate indices (convert from ASSIMP faces
		//format to an array of floats, suitable for 
		//OpenGL)
		unsigned int* generateFaces(aiFace* assimpFaceArray, int numFaces);
	



};
