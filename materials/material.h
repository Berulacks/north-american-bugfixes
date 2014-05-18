#include "../program.h"
//#include "../storage.h"

class Storage;

class Material
{
	public:
		Program* shader;

		Material(Program* _shader)
		{
			//new Program("../shaders/vertex.vs", "../shaders/fragment.fs");
			//initProgram();
			
			shader = _shader;
			

		};

		//Initialize the program object that will
		//store our compiled shaers (one program
		//per material? Shared programs?)
		virtual void initProgram();
		//This handles the initialization of our buffers
		//and toggles vertex attrib's on or off
		virtual void toggleBuffers();
		//Update the uniforms in our shader object
		//Each material should be able to extract the required
		//information from each object for updating each uniform.
		//virtual void updateUniforms(Object dataStore);

};
