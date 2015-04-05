#pragma once
#include "material.h"

//A struct to store info on individual meshes
//(assuming they've already been loaded onto the gpu and
//are ready for rendering)
//(created in Storage::loadModel() )
struct MeshInfo
{
    std::string name;
    GLuint vao, vertexBuffer, indexBuffer, normalsBuffer, uvBuffer, boundingBoxBuffer;

    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int matIndex;

    //Our bounding box's width, height, and depth
    //From the model's origin
    glm::vec3 boundingBox;
};

//A simple struct to store
//raw mesh data. Only used
//to store raw data read from disk
//(created in Storage::readModel() )
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

        unsigned int numMeshes(void) { return meshes.size() ; };
};

//Models are scenes that have their
//corresponding buffers already filled
//out. So, if something has a model, it
//means it's ready to be rendered.
class Model
{

    public:
        //Materials for each mesh
        std::vector<Material> materials;
        Model() {}; //this is horrible and you should feel bad
        //Model(const aiScene* _scene);
        Model( ModelData data );
        //const aiScene* getScene(void) { return scene; };
      
        MeshInfo getMeshInfo(int index) { return bufferIDs[index]; };
        unsigned int numMeshInfos(void) { return bufferIDs.size(); };
        GLuint getVAO(int index) { return bufferIDs[index].vao; };
        unsigned int numMeshes(void)  { return bufferIDs.size(); };
        
        //Generate indices (convert from ASSIMP faces
        //format to an array of floats, suitable for 
        //OpenGL)
        static std::vector<unsigned int> generateFacesVector(aiFace* assimpFaceArray, int numFaces);

    private:

        //IDs for buffers. For internal use only,
        //I guess
        std::vector<MeshInfo> bufferIDs;

        //Create necessary buffer objects
        //and load the actual model data into
        //them.
        void setUpBuffers(ModelData data);

};
