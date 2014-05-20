#include "engine.h"

bool Engine::init( int argc, const char* argv[] )
{
	initSDL();
	renderer.initGL();

	printf("Loading main object... \n");

	//Filenames for the shapes to load
	std::vector<const char*> files;
	//files.push_back( "./models/suzanne.obj");
	//files.push_back("./models/sphere/sphere.obj");
	//files.push_back("./models/dragon_recon/dragon_vrip.ply");
	//files.push_back("./models/bunny/reconstruction/bun_zipper.ply");
	//files.push_back( "./models/dabrovic/sponza.obj");
	
	if(argc >= 2)
	{
		files.push_back(argv[1]);
	}
	else
	{
		printf("[ERROR] Incorrect number of arguments! (%i)\n", argc);
		exit(1);
	}

	printf("Loading model!");
	storage.loadModel( files[0] );

	SDL_GL_SetSwapInterval(1);

	printf("Completed initialization!\n");

	loop( SDL_GetTicks() );

	return 1;
}

void Engine::initSDL()
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

	mainWindow = SDL_CreateWindow("Pedestal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	SDL_SetRelativeMouseMode( SDL_TRUE );

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

}

void Engine::loop(int lastFrame)
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
		timeStepsToProcess -= deltaT;

	}

	SDL_GL_SwapWindow(mainWindow);


	if(active)
		loop(currentTime);
	else
		quit();

}

void Engine::processEvents()
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
					renderer.toggleFullScreen();

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

void Engine::quit()
{
	printf("\nQuitting!\n");
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();

}

Engine::Engine()
{
}
