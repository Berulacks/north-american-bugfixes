#include "storage.h"
#include "object.h"

//A renderer object stores necessary scene data, such 
//as the camera and projection matrices
class Renderer
{
	public:
		//Camera values
		float xRot = 0.0f;
		float yRot = 0.0f;
		glm::vec3 cameraPos = glm::vec3();
		glm::mat4 camera;

		bool isFullScreen;

		Renderer();
		void render(std::vector<Object> objects);

		bool initGL(void);
		bool initSDL(void);
		void toggleFullScreen(void);

		void setActiveProgram(Program* toSet);
		void updateProjection(glm::mat4 projection);
	private:

		glm::mat4 projection;

		SDL_GLContext glContext;
		SDL_Window* mainWindow;

		Program* activeProgram;

		void updateUniforms(Object obj, Program* program = NULL);

		void updateCamera();
};
