#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <iostream>
#include <fstream>
#include <map>

#include "tiny_obj_loader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//#include <string>

//#include "teapot.h"

struct object
{
	std::vector<tinyobj::shape_t> shapes;
	glm::mat4 transform;
};

class Base
{

	SDL_GLContext glContext;
	SDL_Window* mainWindow;

	GLuint program;

        std::map<const char*,GLuint> gpuLocations;

	glm::mat4 camera;
	glm::mat4 projection;
	glm::mat4 model;

	//std::vector<tinyobj::shape_t> shapes;
	std::vector< object > objs;



	int active = 1;
	//60 fps (in milliseconds)
	//(techincally 16.667, but
	//we need to stick with ints
	//because SDL
	//(using constant dT to
	//prevent explosions)
	const int deltaT = 16;
	int timeStepsToProcess = 0;

	bool hasTexture;

	public:
		Base();
		bool init(void);
		void quit(void);
		bool readFile(std::string filename, std::string* target);

	private:
		void render(void);
		void processEvents(void);
		void loop(int lastFrame);
		void physics(void);

		bool initGL(void);
		bool initShader(GLenum type, std::string file);

		bool checkGLErrors(const char* description="");

		//For when our .obj's don't have normals
		void generateNormals(tinyobj::mesh_t *mesh);
		void initBuffers(void);



};
