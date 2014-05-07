#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

#include "program.h"

struct object
{
	const aiScene* scene;
	glm::mat4 transform;

	glm::vec3 position()
	{
		return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	}

	void translate( glm::vec3 translateBy )
	{
		glm::translate( transform, translateBy );
	}
};

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
