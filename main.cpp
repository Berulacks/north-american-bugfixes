#include "engine.h"

int main( int argc, const char* argv[] )
{
	Engine program;
	if( program.init(argc, argv) )
		return 0;
	return 1;
	
}
