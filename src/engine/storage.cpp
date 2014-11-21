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

	textureIDs.emplace( std::string("RGBv"), tex );

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

	std::string base = "./textures/";
	base.append( std::string(filePath) );
	printf("Loading texture %s ... \n", base.c_str());
	unsigned char* pixels = SOIL_load_image(base.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    const int size = glm::abs( width* 3 * height );
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

	textureIDs.emplace( std::string( name ), tex );

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
Model *Storage::loadModel( const char* name )
{
	if(rawModels.size() == 0 || rawModels.find( std::string(name) ) == rawModels.end() )
	{
		printf("[ERROR] Can't load Model: model %s has not been read into the program, yet!\n", name);
	}
	printf("Loading Model %s\n", name);
	ModelData scene = rawModels[std::string(name)];
	Model model = Model( scene );

	/*aiString aiMatName;
	std::string matName;
	printf("Setting up materials...\n");

	//readModel already initializes all materials stored in our aiScene
	//...so we just need to check and see if the name of our materials
	//are in our own materials vector, and update them appropriately
	//...again, this section will need to be removed with ASSIMP, eventually.
	for(int i = 0; i < scene->mNumMeshes; i++)
	{
		scene->mMaterials[ scene->mMeshes[i]->mMaterialIndex ]->Get( AI_MATKEY_NAME, aiMatName );
		matName = std::string(aiMatName.C_Str());
		printf("Searching for material %s for mesh %i\n", materials[matName].name.c_str(), i);
		if(materials.count(matName) != 0)
		{
			printf("Found material %s for mesh %i\n", materials[matName].name.c_str(), i);
			
			//model.updateBComboMat( *materials[matName], i );
			model.materials.push_back( materials[matName] );
		}
	}
	model.setUpBuffers();

	Model* pointer = new Model(model);
	printf("Loading model %s into storage!\n", name);
	models.emplace( std::string(name), pointer );*/
    models.emplace( std::string( name ), model );
	
	return &models[ name ];

}

bool Storage::readModel( const char* filePath )
{

	const aiScene* tempScene;

	tempScene = importer.ReadFile( filePath,
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_GenSmoothNormals    | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes         | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate    | // triangulate polygons with more than 3 edges
		aiProcess_SortByPType              | // make 'clean' meshes which consist of a single typ of primitives
		0);
    
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

    //Our data struct to hold all the values
    //we import from assimp
    ModelData rawData;

    //Read materials
	for(int i = 0; i < tempScene->mNumMaterials; i++)
	{
		printf("Initializing material %i...\n", i);

        Material toAdd = initMaterial( tempScene->mMaterials[i], shader );

        rawData.materials.push_back( toAdd );
	}

    //Read meshes
    for(int i = 0; i < tempScene->mNumMeshes; i++)
    {
            MeshData rawMesh;

            rawMesh.name = std::string( tempScene->mMeshes[i]->mName.C_Str() );
            if( rawMesh.name.size() <= 0 )
            {
                rawMesh.name = std::string( "NAMELESS_MESH" );
            }

            int numVertices;
            //Vertices
            if( tempScene->mMeshes[i]->HasPositions() )
            {
                    numVertices = tempScene->mMeshes[i]->mNumVertices;

                    for(int v = 0; v < numVertices; v++)
                    {
                            glm::vec3 vertex;
                            
                            vertex.x = tempScene->mMeshes[i]->mVertices[v].x;
                            vertex.y = tempScene->mMeshes[i]->mVertices[v].y;
                            vertex.z = tempScene->mMeshes[i]->mVertices[v].z;

                            rawMesh.positions.push_back( vertex );
                    }
            }

            //Indices
            if( tempScene->mMeshes[i]->HasFaces() )
            {

                    rawMesh.indices = generateFacesVector( tempScene->mMeshes[i]->mFaces, tempScene->mMeshes[i]->mNumFaces );

            }

            //Normals
            if( tempScene->mMeshes[i]->HasNormals() )
            {
                    for(int n = 0; n < numVertices; n++)
                    {
                            glm::vec3 normal;

                            normal.x = tempScene->mMeshes[i]->mNormals[n].x;
                            normal.y = tempScene->mMeshes[i]->mNormals[n].y;
                            normal.z = tempScene->mMeshes[i]->mNormals[n].z;

                            rawMesh.normals.push_back( normal );
                    }

            }

            //Texture Coordinates
            //TODO: Change this from "0" to something that makes sense
            if( tempScene->mMeshes[i]->HasTextureCoords( 0 ) )
            {
                    for(int t = 0; t < numVertices; t++)
                    {
                            rawMesh.UVs.push_back( tempScene->mMeshes[i]->mTextureCoords[0][t].x );
                            rawMesh.UVs.push_back( tempScene->mMeshes[i]->mTextureCoords[0][t].y );
                    }

            }

            //MATERIAL FOR MESH:
            rawMesh.material = rawData.materials[tempScene->mMeshes[i]->mMaterialIndex];

            rawData.meshes.push_back(rawMesh);
    }

	//rawModels.emplace( std::string(filePath), tempScene );
	rawModels.emplace( std::string(filePath), rawData );
	printf("Added raw model %s to vector\n", filePath);
	

	printf("File %s has %lu shapes:\n", filePath, rawData.meshes.size() );
	for(int j = 0; j < rawData.meshes.size() ; j++ )
		printf("Shape %i has %lu vertices, and %lu indices\n", j, rawData.meshes[j].positions.size(), rawData.meshes[j].indices.size() );

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
        std::string errorType;

        switch(error)
        {
            case GL_INVALID_OPERATION:
                errorType = "[GL_INVALID_OPERATION]";
                break;
            case GL_INVALID_ENUM:
                errorType = "[GL_INVALID_ENUM]";
                break;
            case GL_INVALID_VALUE:
                errorType = "[GL_INVALID_VALUE]";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorType = "[GL_INVALID_FRAMEBUFFER_OPERATION]";
                break;
            case GL_OUT_OF_MEMORY:
                errorType = "[GL_OUT_OF_MEMORY]";
                break;
            default:
                errorType = "[UNKOWN_TYPE]";
        }
        
		printf("[ERROR]%s@[%s]: %s\n", errorType.c_str(), description, gluErrorString(error));

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

//TODO: Convert the aiMaterial* to a custom data structure, and have initMaterial use that. Reasoning: when we're loading in materials directly from .mtl files, we might not want to use ASSIMP. Though, if ASSIMP can handle doing that, then this is totally okay.
//TODO: Learn more about how ASSIMP loads materials...
Material Storage::initMaterial( aiMaterial* material, Program* shader )
{
	Material mat = Material(shader);
	mat.updateVariables( material );

	printf("Checking texture for material %s...\n", mat.name.c_str());
	if(mat.texDiffuse_name.compare("NONE") == 0)
	{
		printf("No texture found!\nGenerating our own...\n");
		mat.texDiffuse = createTexture( {0.7f,0.5f,0.7f} );

	}
	else	
		//Have we already loaded our texture?
		if( textureIDs.find( mat.texDiffuse_name ) == textureIDs.end() )
		{
			printf("Texture %s for material %s not already loaded, loading...\n", mat.texDiffuse_name.c_str(), mat.name.c_str());
			loadTexture( mat.texDiffuse_name.c_str(), mat.texDiffuse_name.c_str() );
			mat.texDiffuse = textureIDs[ mat.texDiffuse_name ];
		}

	printf("Adding material %s to storage...\n", mat.name.c_str());
	materials.emplace(std::string(mat.name.c_str()), mat);
	printf("Just added material %s to storage, we now have %lu materials.\n", mat.name.c_str(), materials.size());

	//Because Load/CreateTexture for some odd reason wipes
	//the name of mat
	aiString texPath;  
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
	mat.texDiffuse_name = std::string(texPath.data);

	printf("Material '%s' loaded, and ready!\n", mat.name.c_str());

	return mat;
}

void Storage::initMaterials()
{
	Program* prog = new Program("./shaders/vertex.vs", "./shaders/fragment.fs");
	storeProgram( *prog );
	Material mat(prog);
	//mat.updateVariables( 
}


std::vector<unsigned int> Storage::generateFacesVector(aiFace* assimpFaceArray, int numFaces)
{
	// create array with faces
	// have to convert from Assimp format to array
    std::vector<unsigned int> faceArray;

	unsigned int faceIndex = 0;

	for (unsigned int t = 0; t <  numFaces; ++t) {
		const aiFace* face =  &assimpFaceArray[t];

		//memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(unsigned int));
		//faceIndex += 3;
        
        faceArray.push_back( face->mIndices[0] );
        faceArray.push_back( face->mIndices[1] );
        faceArray.push_back( face->mIndices[2] );
	}

	return faceArray;

}
