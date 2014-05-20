#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>*/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

//#include "materials/material.h"
//#include "storage.h"
//#include "object.h"

class Program
{

	public:
		Program(const char* vertexShader, const char* fragShader);
		Program(const char* vertexShader, const char* fragShader, std::vector<const char*> uniforms, std::vector<const char*> attribs);

		GLuint getUniform(const char* name);
		GLuint getAttrib(const char* name);
		GLuint getID(void);
		bool isReady(void);

		//void updateUniforms(glm::mat4 camera, glm::mat4 projection, Object obj);

	private:
		GLuint id;
		bool ready;

		std::map<const char*, GLuint> attribLocations;
		std::map<const char*, GLuint> uniformLocations;

		bool initShader(GLenum type, std::string file);
		bool readFile(std::string filename, std::string* target);

		static bool checkGLErrors(const char* description="");


};
