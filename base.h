#include "renderer.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Base
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
		Base();
		bool init( int argc, const char* argv[] );
		void quit(void);

	private:
		void render(void);
		void processEvents(void);
		void loop(int lastFrame);
		void initSDL(void);
};
