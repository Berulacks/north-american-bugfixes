//#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
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

#include "storage.h"
#include "object.h"

//A renderer object stores necessary scene data, such as camera
class Renderer
{
	public:
		//Camera values
		float xRot = 0.0f;
		float yRot = 0.0f;
		glm::vec3 cameraPos = glm::vec3();
		glm::mat4 camera;

		bool isFullScreen;

		Renderer();
		void render(std::vector<Object> objects);

		bool initGL(void);
		bool initSDL(void);
		void toggleFullScreen(void);

		void setActiveProgram(Program* toSet);
		void updateProjection(glm::mat4 projection);
	private:

		glm::mat4 projection;

		SDL_GLContext glContext;
		SDL_Window* mainWindow;

		Program* activeProgram;

		void updateUniforms(Object obj, Program* program = NULL);

		void updateCamera();
};
