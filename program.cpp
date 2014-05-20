#include "program.h"

Program::Program(const char* vertexShader, const char* fragShader)
{
	ready = false;
	id = glCreateProgram();
	if(initShader(GL_VERTEX_SHADER, vertexShader) && initShader(GL_FRAGMENT_SHADER, fragShader))
	{
		glLinkProgram(id);
		ready = true;
	}
}

Program::Program(const char* vertexShader, const char* fragShader, std::vector<const char*> uniforms, std::vector<const char*> attribs)
{
	ready = false;
	id = glCreateProgram();
	if(initShader(GL_VERTEX_SHADER, vertexShader) && initShader(GL_FRAGMENT_SHADER, fragShader))
	{
		glLinkProgram(id);
		for(int i = 0; i < uniforms.size(); i++)
			uniformLocations.insert( std::pair<const char*, GLuint>(uniforms[i], getUniform(uniforms[i]) ) );

		for(int i = 0; i < attribs.size(); i++)
			attribLocations.insert( std::pair<const char*, GLuint>(attribs[i], getAttrib(attribs[i]) ) );
		ready = true;
	}
}

bool Program::initShader(GLenum type, std::string file)
{
	GLuint shader = glCreateShader( type );
	checkGLErrors("initShader: created shader");

	std::string source;

	if(!readFile(file, &source))
	{
		printf("ERROR: Could not read file %s", file.c_str());
		return false;
	}

	glShaderSource( shader, 1, (const GLchar**)&source, NULL );
	checkGLErrors("initShader: set shader source");
	

	glCompileShader( shader );
	checkGLErrors("initShader: compiled shader");

	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderCompiled ); 
	if( shaderCompiled != GL_TRUE) 
	{ 
		printf( "ERROR: Unable to compile shader %d!\n\nSource:\n%s\n", shader, source.c_str() );
		int infoLength = 0;
		int maxL = infoLength;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxL);
		char* infoLog = new char[maxL];

		glGetShaderInfoLog(id, maxL, &infoLength, infoLog);
		if (infoLength > 0)
			printf("GLSHADERIV:%s\n", infoLog);

		glDeleteShader( shader );
		shader = 0;

		return false;
	}

	glAttachShader( id, shader );
	checkGLErrors("initShader: attached shader");

	return true;
}

GLuint Program::getUniform(const char* name)
{
	return glGetUniformLocation( id, name );
}

GLuint Program::getAttrib(const char* name)
{
	return glGetAttribLocation( id, name );
}

bool Program::readFile(std::string filename, std::string* target)
{

	std::ifstream shaderSource(filename.c_str());

	target->assign( ( std::istreambuf_iterator< char >( shaderSource ) ), std::istreambuf_iterator< char >() );
	if(target->length() > 0)
		return true;

	return false;

}

bool Program::checkGLErrors(const char* description)
{
	GLenum error = glGetError();
	bool hadError = false;

	while(error != GL_NO_ERROR)
	{
		printf("[ERROR]@[%s]: %s\n", description, gluErrorString(error));
		error = glGetError();
		hadError = true;
	}

	return hadError;
}

GLuint Program::getID()
{
	return id;
}
bool Program::isReady()
{
	return ready;
}

