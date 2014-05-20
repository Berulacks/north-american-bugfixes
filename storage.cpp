#include "storage.h"

Storage::Storage()
{
}

bool Storage::loadTexture(const char* filePath, const char* name)
{

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
	checkGLErrors("Setting TexImage2d\n");
	SOIL_free_image_data(pixels);
	printf("Loaded!\n");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	checkGLErrors("Setting texture wrap mode");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLErrors("Setting texture filtering mode");

	textureIDs.insert( std::pair<const char*, GLuint>(name, tex) );

	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}


bool Storage::readFile(std::string filename, std::string* target)
{

	std::ifstream shaderSource(filename.c_str());

	target->assign( ( std::istreambuf_iterator< char >( shaderSource ) ), std::istreambuf_iterator< char >() );
	if(target->length() > 0)
		return true;

	return false;

}

bool Storage::loadModel( const char* filePath )
{

	// Create an instance of the Importer class
	Assimp::Importer importer;
	const aiScene* tempScene;

	tempScene = importer.ReadFile( filePath,
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_GenSmoothNormals    | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes         | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate    | // triangulate polygons with more than 3 edges
		aiProcess_SortByPType              | // make 'clean' meshes which consist of a single typ of primitives
		0);
	//
	// If the import failed, report it
	if( !tempScene)
	{
		printf("[ERROR] Could not import file %s: %s\n", filePath, importer.GetErrorString());
		return false;
	}

	printf("Loaded file %s.\n", filePath);


	printf("Material count : %i\n", tempScene->mNumMaterials);

	//TODO: REMOVE THIS IF/ELSE BLOCK
	//AND ABSTRACT AWAY SHADER STUFF
	Program* shader;
	if(programs.size() == 0)
	{
		printf("No shaders found, creating one for materials...\n");
		shader = new Program("./shaders/vertex.vs", "./shaders/fragment.fs");
		storeProgram ( *shader );
	}	
	else
		shader = &programs[0];

	for(int i = 0; i < tempScene->mNumMaterials; i++)
	{
		printf("Initializing material %i...\n", i);
		initMaterial( tempScene->mMaterials[i], shader );
	}

	rawModels.push_back( tempScene );
	printf("Added to vector\n");
	

	printf("File %s has %i shapes:\n", filePath, tempScene->mNumMeshes);
	for(int j = 0; j < tempScene->mNumMeshes ; j++ )
		printf("Shape %i has %i vertices, and %i indices\n", j, tempScene->mMeshes[j]->mNumVertices, tempScene->mMeshes[j]->mNumFaces);

	//delete tempScene;


	return true;
}

Material Storage::getMaterial ( const char* name )
{
	return materials[name];
}

bool Storage::checkGLErrors(const char* description)
{
	GLenum error = glGetError();
	bool hadError = false;

	while(error != GL_NO_ERROR)
	{
		printf("[ERROR]@[%s]: %s\n", description, gluErrorString(error));
		error = glGetError();
		hadError = true;
	}

	return hadError;
}

bool Storage::storeProgram( Program toAdd )
{
	for(int i = 0; i < programs.size(); i++)
		if(programs[i].getID() == toAdd.getID())
			return false;

	programs.push_back( toAdd );
	return true;
}

bool Storage::initMaterial( aiMaterial* material, Program* shader )
{
	Material mat(shader);
	mat.updateVariables( material, textureIDs );

	if(std::find( textures.begin(), textures.end(), mat.texDiffuse_name ) == textures.end())
	{
		printf("Texture %s for material %s not already loaded, loading...\n", mat.texDiffuse_name, mat.name);
		loadTexture( mat.texDiffuse_name, mat.texDiffuse_name );
	}
	materials.insert( std::pair< const char *, Material >(mat.name, mat) );
	printf("Material %s loaded, and ready!\n", mat.name);

	return true;
}
void Storage::initMaterials()
{
	Program* prog = new Program("./shaders/vertex.vs", "./shaders/fragment.fs");
	storeProgram( *prog );
	Material mat(prog);
	//mat.updateVariables( 
}
