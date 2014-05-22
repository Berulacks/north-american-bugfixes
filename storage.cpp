#include "storage.h"

Storage::Storage()
{
}

GLuint Storage::createTexture( glm::vec3 colour )
{
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glm::vec3 pixels[4] = {colour, colour, colour, colour};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
	checkGLErrors("Generating custom texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	checkGLErrors("Setting custom texture wrap mode");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLErrors("Setting custom texture filtering mode");

	textureIDs.insert( std::pair<const char*, GLuint>("RGBv", tex) );

	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
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

	delete[] finalPixels;

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

//Actually load a raw model's info into GPU memory.
//After this step, a Model object is produced and
//ready to be rendered. (this is the preferred way
//to create models)
Model Storage::loadModel( const char* name )
{
	const aiScene* scene = rawModels[name];
	Model model = Model(rawModels[name]);

	const char* matName;

	//readModel already initializes all materials stored in our aiScene
	//...so we just need to check and see if the name of our materials
	//are in our own materials vector, and update them appropriately
	//...again, this section will need to be removed with ASSIMP, eventually.
	for(int i = 0; i < scene->mNumMeshes; i++)
	{
		scene->mMaterials[ scene->mMeshes[i]->mMaterialIndex ]->Get( AI_MATKEY_NAME, matName );
		if(materials.count(matName) != 0)
			model.updateBComboMat( *materials[matName], i );
	}

	Model* pointer = new Model(model);
	models.insert( std::pair<const char*, Model*>( name, pointer ) );
	
	return model;

}
bool Storage::readModel( const char* filePath )
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

	rawModels.insert( std::pair< const char* , const aiScene* > (filePath, tempScene) );
	printf("Added to vector\n");
	

	printf("File %s has %i shapes:\n", filePath, tempScene->mNumMeshes);
	for(int j = 0; j < tempScene->mNumMeshes ; j++ )
		printf("Shape %i has %i vertices, and %i indices\n", j, tempScene->mMeshes[j]->mNumVertices, tempScene->mMeshes[j]->mNumFaces);

	//delete tempScene;


	return true;
}

Material Storage::getMaterial ( const char* name )
{
	return *materials[name];
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
	Material *mat = new Material(shader);
	mat->updateVariables( material );

	printf("Checking texture for material %s...\n", mat->name.c_str());
	if(mat->texDiffuse_name.compare("NONE") == 0)
	{
		printf("No texture found!\nGenerating our own...\n");
		mat->texDiffuse = createTexture( {0.5f,0.5f,0.5f} );

	}
	else	
		if(std::find( textures.begin(), textures.end(), mat->texDiffuse_name ) == textures.end())
		{
			printf("Texture %s for material %s not already loaded, loading...\n", mat->texDiffuse_name.c_str(), mat->name.c_str());
			loadTexture( mat->texDiffuse_name.c_str(), mat->texDiffuse_name.c_str() );
			mat->texDiffuse = textureIDs[ mat->texDiffuse_name.c_str() ];
		}

	materials.insert( std::pair<const char*, Material*>(mat->name.c_str(), mat) );

	//Because Load/CreateTexture for some odd reason wipes
	//the name of mat
	aiString texPath;  
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
	mat->texDiffuse_name = std::string(texPath.data);

	printf("Material '%s' loaded, and ready!\n", mat->name.c_str());
	//delete mat;

	return true;
}
void Storage::initMaterials()
{
	Program* prog = new Program("./shaders/vertex.vs", "./shaders/fragment.fs");
	storeProgram( *prog );
	Material mat(prog);
	//mat.updateVariables( 
}
