#include "model.h"

Model::Model(const aiScene* _scene, Material* mat )
{
	//aiScene temp = *_scene;
	printf("SETTING SCENE\n");
	scene = _scene;
	printf("NUM INDICES %i\n", _scene->mMeshes[0]->mFaces[0].mNumIndices);

	if( mat != NULL )
		material = mat;
	//else
	//	material = 
	
	setUpBuffers();
}


void Model::setUpBuffers()
{
	GLuint vao, vbo, nbo, ibo, tbo;
	glUseProgram( material->shader->getID() );

	//Lets start with meshes
	for( int i = 0; i < scene->mNumMeshes; i++)
	{

		BufferCombo buffers;
		buffers.name = scene->mMeshes[i]->mName.C_Str();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		buffers.vao = vao;
		//Storage::checkGLErrors("VAO creation");

		//Vertices
		glGenBuffers(1, &vbo);
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBufferData( GL_ARRAY_BUFFER, scene->mMeshes[i]->mNumVertices * 3 * sizeof(float), scene->mMeshes[i]->mVertices, GL_STATIC_DRAW);
		buffers.vertices = vbo;
		//Indices
		glGenBuffers(1, &ibo);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
		unsigned int* indices = generateFaces( scene->mMeshes[i]->mFaces, scene->mMeshes[i]->mNumFaces );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, scene->mMeshes[i]->mNumFaces * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		buffers.indices = ibo;
		delete[] indices;

		//Normals
		glGenBuffers(1, &nbo);
		glBindBuffer( GL_ARRAY_BUFFER, nbo );
		glBufferData( GL_ARRAY_BUFFER, scene->mMeshes[i]->mNumVertices * 3 * sizeof(float), scene->mMeshes[i]->mNormals, GL_STATIC_DRAW);
		buffers.normals = nbo;

		//Texture coords
		glGenBuffers(1, &tbo);
		glBindBuffer( GL_ARRAY_BUFFER, tbo );
		
		//float *texCoords = (float *)malloc(sizeof(float)*2*scene->mMeshes[i]->mNumVertices);
		float *texCoords = new float[2 * scene->mMeshes[i]->mNumVertices];

		if(scene->mMeshes[i]->HasTextureCoords(0))
		{

			for (unsigned int k = 0; k < scene->mMeshes[i]->mNumVertices; ++k) {

				texCoords[k*2]   = scene->mMeshes[i]->mTextureCoords[0][k].x;
				texCoords[k*2+1] = scene->mMeshes[i]->mTextureCoords[0][k].y; 
			}

		}
		else
		{
			//We don't have any texcoords, so lets generate our own.
			for (unsigned int k = 0; k < scene->mMeshes[i]->mNumVertices; ++k) 
			{
				texCoords[k*2] = scene->mMeshes[i]->mVertices[k].x;
				texCoords[k*2+1] = scene->mMeshes[i]->mVertices[k].z;
			}

		}
		glBufferData( GL_ARRAY_BUFFER, scene->mMeshes[i]->mNumVertices * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);

		buffers.texturecoords = tbo;
		delete[] texCoords;

		//buffers.matIndex = scene->mMeshes[i]->mMaterialIndex;

		bufferIDs.push_back( buffers );

		glVertexAttribPointer( material->shader->getAttrib("tex_in"), 2, GL_FLOAT, 0, 0, 0);
		glVertexAttribPointer( material->shader->getAttrib("theN"), 3, GL_FLOAT, 0, 0, 0 );
		glVertexAttribPointer( material->shader->getAttrib("theV"), 3, GL_FLOAT, 0, 0, 0 );

		glEnableVertexAttribArray(material->shader->getAttrib("theV"));
		glEnableVertexAttribArray(material->shader->getAttrib("theN"));
		glEnableVertexAttribArray(material->shader->getAttrib("tex_in"));

		glBindVertexArray( 0 );

	}
	glUseProgram( 0 );


}

Material* Model::getMaterial()
{
	return material;
}


unsigned int* Model::generateFaces(aiFace* assimpFaceArray, int numFaces)
{
	// create array with faces
	// have to convert from Assimp format to array
	unsigned int *faceArray;
	//faceArray = (unsigned int *)malloc(sizeof(unsigned int) * numFaces * 3);
	faceArray = new unsigned int[ numFaces * 3 ];
	unsigned int faceIndex = 0;

	for (unsigned int t = 0; t <  numFaces; ++t) {
		const aiFace* face =  &assimpFaceArray[t];

		memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(unsigned int));
		/*faceArray[faceIndex] = face->mIndices[0];
		faceArray[faceIndex+1] = face->mIndices[1];
		faceArray[faceIndex+2] = face->mIndices[2];*/
		faceIndex += 3;
	}

	return faceArray;

}

