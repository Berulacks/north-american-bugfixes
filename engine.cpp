#include "engine.h"
#include "physics.h"

std::vector<Object> objs;

bool Engine::init( int argc, const char* argv[] )
{
	if(!initSDL())
	{
		printf("Could not initiate SDL!\n");
		return false;
	}
	if(!renderer.initGL())
	{
		printf("Could not initialize OpenGL!\n");
		return false;
	}

	/*storage.readModel( files[0] );
	Model mod = storage.loadModel( files[0] );
	Object sphere = Object(&mod);
	sphere.translateBy( {0.0f,0.0f,5.0f} );
	registerObject( &sphere );
	
	printf("Okay, our model is supposedly loaded, lets check it for some info:\n");
	printf("Our model has %i meshes.\n", mod.numMeshes() );
	printf("The first mesh of our model is called %s\n", mod.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", mod.materials[0].name.c_str());

	printf("\nOkay! Let's give rendering this mother a shot!\n");*/

	SDL_GL_SetSwapInterval(1);

	printf("Completed initialization!\n");

	return true;
}

void Engine::start(int lastFrame)
{
	if( lastFrame == 0)
		lastFrame = SDL_GetTicks();

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


	//Sometimes we might have some timeSteps
	//left over to process, what with rendering
	//perhaps taking too long. This (hopefully)
	//mitigates that.
	while(timeStepsToProcess >= deltaT)
	{

		Physics::updatePhysics( objs, physT );
		timeStepsToProcess -= deltaT;
		for(int i = 0; i < functions.size(); i++)
			(*functions[i])(physT);

	}

	renderer.render(objs);
	SDL_GL_SwapWindow(mainWindow);

	if(active)
		start(currentTime);
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
					renderer.toggleFullScreen(mainWindow);
				if(key == SDLK_UP)
					objs[0]->setScale( {5,5,5} );
				if(key == SDLK_DOWN)
					objs[0]->setScale( {1,1,1} );
				if(key == SDLK_b)
					objs[0]->renderBoundingBox = !objs[0]->renderBoundingBox;

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
bool Engine::registerCallback( Callback function )
{
	if( std::find(functions.begin(), functions.end(), function) == functions.end())
	{
		functions.push_back( function );
		printf("Added callback!\n");
		return true;
	}
	return false;

}
bool Engine::registerObject(Object* toAdd)
{
	if( std::find(objs.begin(), objs.end(), toAdd) == objs.end())
	{
		objs.push_back(toAdd);
		return true;
	}
	return false;
}

bool Engine::initSDL()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return false;
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
		return false;
	}	
	printf("Done.\n");

	printf("Creating GL context... ");
	glContext = SDL_GL_CreateContext(mainWindow);
	printf("Done.\n");

	printf("Loading functions... ");

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		return false;
	}
	printf("Functions loaded!\n");
	printf("OpenGL version is %s\n", glGetString(GL_VERSION) );
	printf("GLSL version is %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	return true;

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
