#include "engine.h"

Renderer* myRenderer;
Storage* myStorage;
Engine* myEngine;

void processEvents(float physT)
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

					myEngine->quit();

				if(key == SDLK_w)
					myRenderer->cameraPos -= glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_s)
					myRenderer->cameraPos += glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_a)
					myRenderer->cameraPos -= glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_d)
					myRenderer->cameraPos += glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_f)
					myRenderer->toggleFullScreen(myEngine->getWindow() );

				break;

			case SDL_MOUSEMOTION:

				myRenderer->xRot -= event.motion.xrel * 0.001;
				myRenderer->yRot -= event.motion.yrel * 0.001;
				break;
		}

	}



	if(myRenderer->xRot < -M_PI)
		myRenderer->xRot += M_PI * 2;

	else if(myRenderer->xRot > M_PI)
		myRenderer->xRot -= M_PI * 2;

	if(myRenderer->yRot < -M_PI / 2)
		myRenderer->yRot = -M_PI / 2;
	if(myRenderer->yRot > M_PI / 2)
		myRenderer->yRot = M_PI / 2;

	lookat.x = sinf(myRenderer->xRot) * cosf(myRenderer->yRot);
	lookat.y = sinf(myRenderer->yRot);
	lookat.z = cosf(myRenderer->xRot) * cosf(myRenderer->yRot);

	myRenderer->camera = glm::lookAt(myRenderer->cameraPos, myRenderer->cameraPos + lookat, glm::vec3(0, 1, 0));
}

int main( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init(argc, argv) )
		exit(1);

    myEngine = &program;

	printf("Program initialized, let's add a callback!\n");

	program.registerCallback( processEvents );
	
	myStorage = program.getStorage();
	myRenderer = program.getRenderer();

	std::vector<const char*> files;
	if(argv[1] == NULL)
		files.push_back( "./models/suzanne.obj");
	else
		files.push_back( argv[1] );

	if( !myStorage->readModel(files[0]) )
		program.quit();
	
	Model mod = *( myStorage->loadModel( files[0] ) );
	Object sphere = Object(&mod);
	sphere.translateBy( {0.0f,0.0f,5.0f} );
	program.registerObject( &sphere );

	printf("Okay, our model is supposedly loaded, lets check it for some info:\n");
	printf("Our model has %i meshes.\n", mod.numMeshes() );
	printf("The first mesh of our model is called %s\n", mod.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", mod.materials[0].name.c_str());
	printf("And it looks for a texture called %s\n", mod.materials[0].texDiffuse_name.c_str() );

	program.start( SDL_GetTicks() );

	return 0;
	
}
