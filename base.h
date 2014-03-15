#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
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

#define E_CONSTANT 1.0
#define MASS_1 1.0
#define MASS_2 1.0




struct object
{
	std::vector<tinyobj::shape_t> shapes;
	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec3 scale;

	glm::vec3 orientation;
	glm::vec3 rotVelocity;

	glm::mat4 transform()
	{
		glm::mat4 matrix;

		matrix = glm::scale( glm::mat4(), scale);

		matrix = glm::rotate( matrix, orientation.x, {1.0f,0.0f,0.0f} );
		matrix = glm::rotate( matrix, orientation.y, {0.0f,1.0f,0.0f} );
		matrix = glm::rotate( matrix, orientation.z, {0.0f,0.0f,1.0f} );

		matrix = glm::translate( matrix, position );

		return matrix;
	}

	float mass;

};

struct plane
{
	//A plane is defined
	//by a point and a 
	//normal vector
	glm::vec3 p;
	glm::vec3 n;

	float distanceFromPlane(glm::vec3 q)
	{
		return (glm::dot( q, n )) - glm::dot(n , p);
	}
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

	//Everything is scaled down.
	float sphereRadius = 1.0f;
	float velocity = 0.05f;

	// #justcubethings
	// (the actual cube we use
	// for collision checking)
	plane cubePlanes[6];

	//(the values here are in camera
	//space coordinates. The "model"
	//space coordinates are 1/2)
	glm::mat4 cubeMatrix;
	glm::vec3 cube[16] =
	{ { 2.0f, -2.0f, 2.0f },
	  { -2.0f, -2.0f, 2.0f },
	  { -2.0f, 2.0f, 2.0f },
	  { 2.0f, 2.0f, 2.0f },
	  { 2.0f, -2.0f, 2.0f },
	  { 2.0f, -2.0f, -2.0f},
	  { -2.0f, -2.0f, -2.0f},
	  { -2.0f, -2.0f, 2.0f},
	  { -2.0f, 2.0f, 2.0f},
	  { -2.0f, 2.0f, -2.0f},
	  { -2.0f, -2.0f, -2.0f},
	  { -2.0f, 2.0f, -2.0f},
	  { 2.0f, 2.0f, -2.0f},
	  { 2.0f, 2.0f, 2.0f},
	  { 2.0f, 2.0f, -2.0f},
	  { 2.0f, -2.0f, -2.0f} };

	//Camera values
	float xRot = 0.0f;
	float yRot = 0.0f;
	float mouseX, mouseY;
	glm::vec3 cameraPos = glm::vec3();


	int active = 1;
	//60 fps (in milliseconds)
	//(techincally 16.667, but
	//we need to stick with ints
	//because SDL
	//(using constant dT to
	//prevent explosions)
	//
	const int deltaT = 16;
	//In seconds
	const float physT = 0.06;

	int timeStepsToProcess = 0;

	bool hasTexture;
	bool isFullscreen;


	public:
		Base();
		bool init(void);
		void quit(void);
		bool readFile(std::string filename, std::string* target);

		static float getRandom(float low=0.0f, float high=1.0f);

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

		void toggleFullScreen(void);

		float checkForSphereCollision(object s1, object s2);
		void handleSphereCollision(object* s1, object* s2, float e); 
		void handlePlaneCollision(object* s, plane p);



};
