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
	files.push_back( "./models/suzanne.obj");

	if( !myStorage->readModel(files[0]) )
		program.quit();
	
	Model mod = myStorage->loadModel( files[0] );
	Object sphere = Object(&mod);
	sphere.translateBy( {0.0f,0.0f,5.0f} );
	program.registerObject( &sphere );

	printf("Okay, our model is supposedly loaded, lets check it for some info:\n");
	printf("Our model has %i meshes.\n", mod.numMeshes() );
	printf("The first mesh of our model is called %s\n", mod.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", mod.materials[0].name.c_str());
	printf("And it looks for a texture called%s\n", mod.materials[0].texDiffuse_name.c_str() );

	program.start( SDL_GetTicks() );

	return 0;
	
}
