#include "base.h"

bool Base::init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		quit();
	}
	printf("SDL Initialized!\n");

	//Mesa currently only supports a 3.3 core profile, AFAIK
	//So, being a Linux user with an Intel CPU (and mesa)
	//I'm going to have to stick with OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mainWindow = SDL_CreateWindow("SDL is fun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	SDL_SetRelativeMouseMode( SDL_TRUE );

	isFullscreen = false;

	if(!mainWindow)
	{
		printf("Woops, couldn't create the main window.");
		quit();
	}	
	printf("Done.\n");

	printf("Creating GL context... ");
	glContext = SDL_GL_CreateContext(mainWindow);
	printf("Done.\n");

	printf("Loading functions... ");

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		exit(1);
	}
	printf("Functions loaded!\n");
	printf("OpenGL version is %s\n", glGetString(GL_VERSION) );
	printf("GLSL version is %s\n",  glGetString(GL_SHADING_LANGUAGE_VERSION));

	renderer.initGL();

	printf("Loading main object... \n");

	//Filenames for the shapes to load
	std::vector<const char*> files;
	//files.push_back( "./models/suzanne.obj");
	//files.push_back("./models/sphere/sphere.obj");
	//files.push_back("./models/dragon_recon/dragon_vrip.ply");
	files.push_back("./models/bunny/reconstruction/bun_zipper.ply");
	//files.push_back( "./models/dabrovic/sponza.obj");
	
	// Create an instance of the Importer class
	Assimp::Importer importer;
	const aiScene* tempScene;
	object tempObj;

	for(int i = 0; i < files.size(); i++)
	{
		tempScene = importer.ReadFile( files[i],
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
			printf("[ERROR] Could not import file %s: %s\n", files[i], importer.GetErrorString());
			exit(1);
		}

		printf("Loaded file %s.\n", files[i]);

		tempObj.scene = tempScene;
		printf("Set scene\n");
		tempObj.transform = glm::mat4();
		printf("Set transform\n");
		tempObj.translate( {0,0,-5} );

		printf("Material count : %i\n", tempObj.scene->mNumMaterials);

		if(tempObj.scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			printf("Textures present in object %i, attempting to load...\n", i);

			aiString texPath;
			tempObj.scene->mMaterials[1]->GetTexture( aiTextureType_DIFFUSE, 0,&texPath);
			printf("This is supposed to be the texture's path: %s\n", texPath.C_Str() );
			renderer.loadTexture( texPath.C_Str(), "earthmap" );
		}
		else
			printf("No textures found in object %i\n", i);

		objs.push_back( tempObj );
		printf("Added to vector\n");
		

		printf("Object %i (%s) has %i shapes:\n", i, files[i], tempScene->mNumMeshes);
		for(int j = 0; j < objs[i].scene->mNumMeshes ; j++ )
			printf("Shape %i has %i vertices, and %i indices\n", j, objs[i].scene->mMeshes[i]->mNumVertices, objs[i].scene->mMeshes[i]->mNumFaces);


	}

	SDL_GL_SetSwapInterval(1);

	printf("Completed initialization!\n");

	loop( SDL_GetTicks() );

	return 1;
}

void Base::loop(int lastFrame)
{
	//Lets process our events, first
	processEvents();

	//Because we're using a constant 
	//deltaT with a variable fps
	//we have no choice but to
	//do multiple physics steps if
	//our dF > dT (or else we could
	//potentially pass in really big
	//or really small values to the physics
	//simulation)
	int currentTime = SDL_GetTicks();
	//The change between frames
	int deltaF = currentTime - lastFrame;
	timeStepsToProcess += deltaF;


	while(timeStepsToProcess >= deltaT)
	{
		//Note: Since we're using 
		//a constant deltaT declared
		//in the header file, we're
		//not passing dT as an argument
		physics();
		timeStepsToProcess -= deltaT;

	}

	//printf("Rendering...\n");
	renderer.render(objs);
	SDL_GL_SwapWindow(mainWindow);


	if(active)
		loop(currentTime);
	else
		quit();

}

bool Base::checkGLErrors(const char* description)
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


void Base::physics()
{


}

void Base::processEvents()
{
	SDL_Event event;
	SDL_Keycode key;
	glm::vec3 lookat;

	while (SDL_PollEvent(&event)) 
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if(key == SDLK_SPACE
						||
					key == SDLK_RETURN
						||
					key == SDLK_ESCAPE)

					active = false;

				if(key == SDLK_w)
					renderer.cameraPos -= glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(renderer.camera) * physT;

				if(key == SDLK_s)
					renderer.cameraPos += glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(renderer.camera) * physT;

				if(key == SDLK_a)
					renderer.cameraPos -= glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(renderer.camera) * physT;

				if(key == SDLK_d)
					renderer.cameraPos += glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(renderer.camera) * physT;

				if(key == SDLK_f)
					toggleFullScreen();

				break;

			case SDL_MOUSEMOTION:

				renderer.xRot -= event.motion.xrel * 0.001;
				renderer.yRot -= event.motion.yrel * 0.001;
				break;
		}

	}



	if(renderer.xRot < -M_PI)
		renderer.xRot += M_PI * 2;

	else if(renderer.xRot > M_PI)
		renderer.xRot -= M_PI * 2;

	if(renderer.yRot < -M_PI / 2)
		renderer.yRot = -M_PI / 2;
	if(renderer.yRot > M_PI / 2)
		renderer.yRot = M_PI / 2;

	lookat.x = sinf(renderer.xRot) * cosf(renderer.yRot);
	lookat.y = sinf(renderer.yRot);
	lookat.z = cosf(renderer.xRot) * cosf(renderer.yRot);

	renderer.camera = glm::lookAt(renderer.cameraPos, renderer.cameraPos + lookat, glm::vec3(0, 1, 0));
}

void Base::quit()
{
	printf("\nQuitting!\n");
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();

}

Base::Base()
{
}


bool Base::readFile(std::string filename, std::string* target)
{

	std::ifstream shaderSource(filename.c_str());

	target->assign( ( std::istreambuf_iterator< char >( shaderSource ) ), std::istreambuf_iterator< char >() );
	if(target->length() > 0)
		return true;

	return false;

}

void Base::toggleFullScreen()
{
	int w, h;
	if(!isFullscreen)
	{
		SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		renderer.updateProjection( glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.f) );
		SDL_GetWindowSize(mainWindow, &w, &h);
		printf("Going fullscreen: %ix%i\n", w, h);
		glViewport(0,0,w,h);
		isFullscreen = true;
	}
	else
	{
		SDL_SetWindowFullscreen(mainWindow, 0);
		SDL_SetWindowSize( mainWindow, 800, 600);
		renderer.updateProjection( glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f)) ;
		glViewport(0,0,800,600);
		isFullscreen = false;
	}
}

