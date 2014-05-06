#include "renderer.h"

bool Renderer::initGL()
{
	printf("Initializing openGL...\n");
	glEnable(GL_DEPTH_TEST);
	//GL_ShadeModel(GL_FLAT);

	//..program = glCreateProgram();
	Program defaultProgram = Program("./shaders/vertex.vs", "./shaders/fragment.notex.fs",{"mvp", "mv", "normal_matrix", "LightPosition", "Ld", "Kd"}, {"theV", "theN", "tex_in"});


	//printf("Setting up initial buffers... \n");
	//initBuffers();
	//printf("Done.\n");

	glUseProgram( defaultProgram.getID() );
	checkGLErrors("Use program");

	
	printf("OpenGL initialized!\n");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	return checkGLErrors("End of initGL");;
}
