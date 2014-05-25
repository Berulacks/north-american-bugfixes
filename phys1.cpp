#include "engine.h"

Renderer* myRenderer;
Storage* myStorage;

int main( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init(argc, argv) )
		exit(1);

	printf("Program initialized, let's add a callback!\n");

	//program.registerCallback( myCallback );
	
	myStorage = program.getStorage();
	myRenderer = program.getRenderer();

	std::vector<const char*> files;
	if(argv[1] == NULL)
		files.push_back( "./models/suzanne.obj");
	else
		files.push_back( argv[1] );

	if( !myStorage->readModel(files[0]) )
		program.quit();
	
	Model mod = myStorage->loadModel( files[0] );
	Object sphere = Object(&mod);
	sphere.translateBy( {0.0f,20.0f,15.0f} );
	sphere.isPhysicsObject = true;
	sphere.rotateBy( {0.1, 0.4, 0.3} );
	sphere.collidable = true;
	program.registerObject( &sphere );

	Object sphere2 = Object(&mod);
	sphere2.translateBy( {0.0f,-2.0f,15.0f} );
	sphere.collidable = true;
	program.registerObject( &sphere2 );

	program.start( SDL_GetTicks() );

	return 0;
	
}
