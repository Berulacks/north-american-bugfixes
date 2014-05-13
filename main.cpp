#include "base.h"

int main( int argc, const char* argv[] )
{
	Base program;
	if( program.init(argc, argv) )
		return 0;
	return 1;
	
}
