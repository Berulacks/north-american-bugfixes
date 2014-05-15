#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

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
#include <vector>

#include "materials/material.h"

struct object
{
	const aiScene* scene;
	int vao;
	glm::mat4 transform;
	Material mat;

	glm::vec3 position()
	{
		return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	}

	void translate( glm::vec3 translateBy )
	{
		glm::translate( transform, translateBy );
	}
};

class Program
{

	public:
		Program(const char* vertexShader, const char* fragShader);
		Program(const char* vertexShader, const char* fragShader, std::vector<const char*> uniforms, std::vector<const char*> attribs);

		GLuint getUniform(const char* name);
		GLuint getAttrib(const char* name);
		GLuint getID(void);
		bool isReady(void);

		void updateUniforms(object* model);

	private:
		GLuint id;
		bool ready;

		std::map<const char*, GLuint> attribLocations;
		std::map<const char*, GLuint> uniformLocations;

		bool initShader(GLenum type, std::string file);
		bool readFile(std::string filename, std::string* target);

		static bool checkGLErrors(const char* description="");


};
