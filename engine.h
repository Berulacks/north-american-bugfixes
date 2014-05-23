#include "renderer.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//For functions that our main loop will call.
typedef void (*Callback)();

class Engine
{
	Renderer renderer;
	Storage storage;

	SDL_GLContext glContext;
	SDL_Window* mainWindow;

	GLuint program;

        std::map<const char*,GLuint> gpuLocations;

	glm::mat4 camera;
	glm::mat4 projection;
	glm::mat4 model;

	int active = 1;
	//60 fps (in milliseconds)
	//(techincally 16.667, but
	//we need to stick with ints
	//because SDL
	//(using constant dT to
	//prevent explosions)
	const int deltaT = 16;
	//In seconds
	const float physT = 0.06;

	int timeStepsToProcess = 0;

	public:
		Engine();
		bool init( int argc, const char* argv[] );
		void quit(void);

		//Adds an object to the engine's list of 
		//known objects. Then the engine can
		//determine whether to apply physics,
		//collisions, and render the object
		//Returns true if object was added, false
		//if object is already present in list
		bool registerObject(Object toAdd);

	private:
		std::vector<Object> objs;
		void render(void);
		void processEvents(void);
		void loop(int lastFrame);
		void initSDL(void);
};
