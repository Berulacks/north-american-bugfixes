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
		Renderer();
		void render(void);

		bool initGL(void);
		bool initShader(GLenum type, std::string file, GLuint program);
		void initBuffers(void);
		void toggleFullScreen(void);
	private:
		std::vector<Program> programs;
		bool readFile(std::string filename, std::string* target);
		static bool checkGLErrors(const char* description="");

};
