#include "model.h"


void Model::setUpBuffers()
{
	GLuint vao, vbo, nbo, ibo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	Storage::checkGLErrors("VAO creation");

	//Lets start with meshes
	for( int i = 0; i < scene->mNumMeshes; i++)
	{
		glGenBuffers(1, &vbo);

	}

}


unsigned int* Model::generateFaces(aiFace* assimpFaceArray, int numFaces)
{

	// create array with faces
	// have to convert from Assimp format to array
	unsigned int *faceArray;
	faceArray = (unsigned int *)malloc(sizeof(unsigned int) * numFaces * 3);
	unsigned int faceIndex = 0;

	for (unsigned int t = 0; t <  numFaces; ++t) {
		const aiFace* face =  &assimpFaceArray[t];

		memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(unsigned int));
		faceIndex += 3;
	}

	return faceArray;

}

