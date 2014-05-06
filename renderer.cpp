#include "renderer.h"

void Renderer::render(std::vector<object> objects)
{
	glClearColor(1.0f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;

	for(int i = 0; i < objects.size(); i++)
	{
		//Uniforms
		mv = camera * objects[i].transform;
		mvp = projection * mv;
		rot = (glm::mat3)mv;
		normal = glm::inverseTranspose(rot);

		glUniformMatrix4fv(activeProgram->getUniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp) );
		glUniformMatrix4fv(activeProgram->getUniform("mv"), 1, GL_FALSE, glm::value_ptr(mv) );
		glUniformMatrix3fv(activeProgram->getUniform("normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal) );

		for(int j = 0; j < objects[i].scene->mNumMeshes; j++)
		{

			unsigned int *faceArray = generateFaces(objects[i].scene[0].mMeshes[0]->mFaces, objects[i].scene->mMeshes[j]->mNumFaces);

			//Normals
			glBindBuffer( GL_ARRAY_BUFFER, bufferObjects.at("nbo") );
			glBufferData( GL_ARRAY_BUFFER, objects[i].scene->mMeshes[j]->mNumVertices* 3 * sizeof(float), objects[i].scene->mMeshes[j]->mNormals, GL_STATIC_DRAW);
			glVertexAttribPointer( activeProgram->getAttrib("theN"), 3, GL_FLOAT, 0, 0, 0 );

			if(objects[i].scene->HasTextures())
			{
				float *texCoords = (float *)malloc(sizeof(float)*2*objects[0].scene->mMeshes[0]->mNumVertices);
				for (unsigned int k = 0; k < objects[0].scene->mMeshes[0]->mNumVertices; ++k) {

					texCoords[k*2]   = objects[0].scene->mMeshes[0]->mTextureCoords[0][k].x;
					texCoords[k*2+1] = objects[0].scene->mMeshes[0]->mTextureCoords[0][k].y; 

				}
				//Texture Coords
				glBindBuffer( GL_ARRAY_BUFFER, bufferObjects.at("tbo") );
				glBufferData( GL_ARRAY_BUFFER, objects[0].scene->mMeshes[0]->mNumVertices * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
				glVertexAttribPointer( activeProgram->getAttrib("tex_in"), 2, GL_FLOAT, 0, 0, 0);
				free(texCoords);	
			}

			//Vertices
			glBindBuffer( GL_ARRAY_BUFFER, bufferObjects.at("vbo") );
			glBufferData( GL_ARRAY_BUFFER, objects[i].scene->mMeshes[j]->mNumVertices * sizeof(float) * 3, objects[i].scene->mMeshes[j]->mVertices, GL_STATIC_DRAW );
			glVertexAttribPointer( activeProgram->getAttrib("theV"), 3, GL_FLOAT, 0, 0, 0 );

			//Indices
			//glBufferData( GL_ELEMENT_ARRAY_BUFFER, objs[i].shapes[j].mesh.indices.size() * sizeof(unsigned int), &objs[i].shapes[j].mesh.indices[0], GL_STATIC_DRAW );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, objects[i].scene->mMeshes[j]->mNumFaces * 3 * sizeof(unsigned int), faceArray, GL_STATIC_DRAW );
			free(faceArray);

			glDrawElements(GL_TRIANGLES, objects[i].scene->mMeshes[j]->mNumFaces * 3, GL_UNSIGNED_INT, NULL); 
		}
	}
	checkGLErrors("Post render");

	//SDL_GL_SwapWindow(mainWindow);

}

void Renderer::setActiveProgram(Program toSet)
{
	glUseProgram( toSet.getID() );
	activeProgram = &toSet;
}
bool Renderer::initGL()
{
	printf("Initializing openGL...\n");
	glEnable(GL_DEPTH_TEST);
	//GL_ShadeModel(GL_FLAT);

	Program defaultProgram = Program("./shaders/vertex.vs", "./shaders/fragment.notex.fs",{"mvp", "mv", "normal_matrix", "LightPosition", "Ld", "Kd"}, {"theV", "theN", "tex_in"});


	printf("Setting up initial buffers... \n");
	initBuffers();
	printf("Done.\n");

	// **THESE NEED TO BE ABSTRACTED, LATER**
	//Position of light
	glUniform4fv(activeProgram->getUniform("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) ); 
	checkGLErrors("Init LP uniform");
	//Light constant
	glUniform3fv(activeProgram->getUniform("Kd"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.7) ) ); 
	checkGLErrors("Init Kd uniform");
	//Light intensity
	glUniform3fv(activeProgram->getUniform("Ld"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.9) ) ); 
	checkGLErrors("Init Ld uniform");

	glUniform1i(glGetUniformLocation(activeProgram->getID(), "texSampler"), 0);
	// **
	
	setActiveProgram( defaultProgram );
	checkGLErrors("Use program");
	
	printf("OpenGL initialized!\n");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	return checkGLErrors("End of initGL");;
}


void Renderer::initBuffers()
{

	GLuint vbo;
        GLuint ibo;
	GLuint vao;
	GLuint nbo;
	GLuint tbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	checkGLErrors("VAO creation");

	glGenBuffers( 1, &nbo );
	checkGLErrors("NBO binding");


	glGenBuffers(1, &tbo);

	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );

	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

	glBindVertexArray(vao);
	checkGLErrors("Binding vertex array");

	bufferObjects.insert( std::pair<const char *, GLuint>("vbo", vbo) );
	bufferObjects.insert( std::pair<const char *, GLuint>("ibo", ibo) );
	bufferObjects.insert( std::pair<const char *, GLuint>("vao", vao) );
	bufferObjects.insert( std::pair<const char *, GLuint>("nbo", nbo) );
	bufferObjects.insert( std::pair<const char *, GLuint>("tbo", tbo) );
}

GLuint Renderer::loadTexture( const char* filePath, const char* name )
{

	//Normally, here, we'd want to read the diffuse_texname
	//of our loaded object
	//But SOIL, for some odd reason, won't accept char* variables

	GLuint tex = 0;
	int width, height, channels;
	glGenTextures(1, &tex);
	checkGLErrors("Generating texture");

	glBindTexture(GL_TEXTURE_2D, tex);
	checkGLErrors("Binding texture");

	printf("Loading texture %s ... \n", filePath);
	unsigned char* pixels = SOIL_load_image(filePath, &width, &height, &channels, SOIL_LOAD_AUTO);
	const int size = width* 3 * height;
	unsigned char* finalPixels = new unsigned char[size];

	//Flip the rows of our texture
	//For opengl compatability
	for(int row = 0; row < height; row++)
	{
		for(int col=0; col < width; col++)
		{
			for(int i = 0; i < 3; i++)
			{
				finalPixels[(height-1-row) * width * 3 + col*3 +i] = pixels[row*width*3+col*3+i];
			}
		}

	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, finalPixels);
	SOIL_free_image_data(pixels);
	printf("Loaded!\n");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	checkGLErrors("Setting texture wrap mode");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLErrors("Setting texture filtering mode");

	textures.insert( std::pair<const char*, GLuint>(name, tex) );
	
	return tex;
}

unsigned int* Renderer::generateFaces(aiFace* assimpFaceArray, int numFaces)
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
