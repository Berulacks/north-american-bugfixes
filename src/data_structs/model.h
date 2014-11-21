#pragma once
#include "material.h"

struct BufferCombo
{
	std::string name;
	GLuint vao, vertices, indices, normals, texturecoords, boundingBox;

    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int matIndex;
};

//A simple struct to store
//raw mesh data.
struct MeshData
{
        std::string name = std::string("NAMELESS_MESH");

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
        //Stored in pairs of ints
        //so...
        //x1,y1,x2,y2,x3,y3,...etc.
        std::vector<float> UVs;

        Material material;

        bool hasPositions(void) { return positions.size() > 0; };
        bool hasNormals(void) { return normals.size() > 0; };
        bool hasIndices(void) { return indices.size() > 0; };
        bool hasUVs(void) { return UVs.size() > 0; };
};

struct ModelData
{
        std::vector<MeshData> meshes;
        std::vector<Material> materials;

        uint numMeshes(void) { return meshes.size() ; };
};

//Models are scenes that have their
//corresponding buffers already filled
//out. So, if something has a model, it
//means it's ready to be rendered.
class Model
{
	//The aiScene which
	//contains our actual model data
	//(this needs to eventually be replaced with
	//an internal format, when I phase assimp out)
	const aiScene* scene;
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
		Model() {}; //this is horrible and you should feel bad
		//Model(const aiScene* _scene);
        Model( ModelData data );
		//const aiScene* getScene(void) { return scene; };
      
		BufferCombo getBCombo(int index) { return bufferIDs[index]; };
		unsigned int numBCombos(void) { return bufferIDs.size(); };
		GLuint getVAO(int index) { return bufferIDs[index].vao; };
		unsigned int numMeshes(void)  { return bufferIDs.size(); };

	private:

		//Generate indices (convert from ASSIMP faces
		//format to an array of floats, suitable for 
		//OpenGL)
		static unsigned int* generateFaces(aiFace* assimpFaceArray, int numFaces);
		static std::vector<unsigned int> generateFacesVector(aiFace* assimpFaceArray, int numFaces);
		//Create necessary buffer objects
		//and load the actual model data into
		//them.
		void setUpBuffers(ModelData data);

};
