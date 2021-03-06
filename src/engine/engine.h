#include <functional>
#include "../renderer/renderer.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//Main engine instance.
class Engine
{
    Renderer renderer;
    Storage storage;

    SDL_GLContext glContext;
    SDL_Window* mainWindow;

    std::map<const char*,GLuint> gpuLocations;

    glm::mat4 camera;
    glm::mat4 projection;

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
        //Initializes the engine, renderer, and storage
        bool init( std::string name, int argc, const char* argv[] );

        void quit(void);

        //Starts the main update loop.
        void start(int lastFrame);

        //Add a custom function to our update loop.
        //This is for integrating your own program
        //with the engine's update loop.
        bool registerCallback(std::function<void(float)> function);

        Renderer* getRenderer(void) { return &renderer; };
        Storage* getStorage(void) { return &storage; };

        //Returns the main SDL window the engine has created
        SDL_Window* getWindow(void) { return mainWindow; };

        void setActiveScene( Scene toSet );
        Scene* getActiveScene( void );

    private:
        //The window name/name of this instance
        std::string name;
        //The currently active scene
        Scene activeScene;
        //Outside functions to be appended
        //to our main update loop. These
        //functions will be passed a 
        //deltaT value (in seconds)
        std::vector< std::function<void(float)> > functions;
        //Timer for the program
        std::chrono::steady_clock::time_point beg, end;
        //Render the objects in objs
        void render(void);
        //TODO
        void processEvents(void);
        //Initialize SDL functions and the main window
        bool initSDL(void);
};
