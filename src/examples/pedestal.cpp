#include "pedestal.h"

void Pedestal::processEvents(float physT)
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
                if(key == SDLK_b)
                    obj->renderBoundingBox = !obj->renderBoundingBox ;

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



Pedestal::Pedestal( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init(argc, argv) )
		exit(1);

    myEngine = &program;

	printf("Program initialized, let's add a callback!\n");

    using std::placeholders::_1;
	program.registerCallback( std::bind( &Pedestal::processEvents, this, _1 ) );
	
	myStorage = program.getStorage();
	myRenderer = program.getRenderer();

    myRenderer->cameraPos = {0.0, 3.0, 0.0};

	std::vector<const char*> files;

	if(argv[1] == NULL)
		files.push_back( "./models/suzanne.obj");
	else
    {
		files.push_back( argv[1] );
    }

	if( !myStorage->readModel(files[0]) )
    {
        printf("Could not load model ' %s '!\n",files[0]);
		program.quit();
    }
	
    Model model = myStorage->loadModel( files[0] );

	printf("Okay, our modelel is supposedly loaded, lets check it for some info:\n");
	printf("Our modelel has %i meshes.\n", model.numMeshes() );
	printf("The first mesh of our modelel is called %s\n", model.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", model.materials[0].name.c_str());
	printf("And it looks for a texture called %s\n", model.materials[0].texDiffuse_name.c_str() );

    if( !myStorage->readModel( "./models/plane/plane.obj" ) )
        program.quit();

    Object plane = Object( myStorage->loadModel( "./models/plane/plane.obj" ) );
    plane.setTranslation( {0, -2, 4} );
    program.registerObject( &plane );

	obj = new Object(model);
	obj->translateBy( {0.0f,0.0f,5.0f} );
	myEngine->registerObject( obj );

	program.start( SDL_GetTicks() );
	
}


int main( int argc, const char* argv[] )
{
    Pedestal pedestal(argc, argv);
    return 0;
}
