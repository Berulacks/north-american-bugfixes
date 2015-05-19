#include "../engine/storage.h"
#include "../data_structs/displayobject.h"

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
        void render(std::vector<DisplayObject*> objects);

        bool initGL(void);
        void toggleFullScreen(SDL_Window* mainWindow);

        void setActiveProgram(Program* toSet);
        void updateProjection(glm::mat4 projection);
    private:

        MeshInfo boundingBox;
        
        //A basic pair of shaders
        //that just renders incoming
        //vertices.
        Program* simplePr;
        
        //The vertex array object
        //for rendering object 
        //bounding boxes
        GLuint bBoxVao;

        glm::mat4 projection;

        Program* activeProgram;

        void updateUniforms(DisplayObject obj, Program* program = NULL);

        void updateCamera();
};
