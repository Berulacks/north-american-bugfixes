#include "engine.h"

int main( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init(argc, argv) )
		exit(1);

	printf("Program initialized, let's add a callback!\n");

	//program.registerCallback( myCallback );

	program.start( SDL_GetTicks() );



	return 0;
	
}
