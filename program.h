#pragma once

#ifndef OGL_LD
#define OGL_LD

#include "libs/gl_core_3_3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#endif

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

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
