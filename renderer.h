#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#include <iostream>
#include <fstream>
#include <map>

#include "program.h"


class Renderer
{
	public:
		//Camera values
		float xRot = 0.0f;
		float yRot = 0.0f;
		glm::vec3 cameraPos = glm::vec3();
		glm::mat4 camera;

		Renderer();
		void render(std::vector<object> objects);

		bool initGL(void);
		bool initShader(GLenum type, std::string file, GLuint program);
		void initBuffers(void);
		void toggleFullScreen(void);

		GLuint loadTexture(const char* filePath, const char* name);

		static unsigned int* generateFaces(aiFace* assimpFaceArray, int numFaces);

		void setActiveProgram(Program* toSet);
		void updateProjection(glm::mat4 projection);
	private:

		glm::mat4 projection;

		Program* activeProgram;
		std::vector<Program*> programs;
		std::map<const char*, GLuint> textures;
		std::map<const char*, GLuint> bufferObjects;
		bool readFile(std::string filename, std::string* target);
		static bool checkGLErrors(const char* description="");

		void updateUniforms(void);

};
