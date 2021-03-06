#include "engine.h"

std::vector<Object> objs;

bool Engine::init( std::string name, int argc, const char* argv[] )
{
    using namespace std::chrono;

    this->name = name;

    steady_clock::time_point inter;
    duration<double> diff;
    beg = steady_clock::now();

    if(!initSDL())
    {
        printf("Could not initialize SDL!\n");
        return false;
    }
    else
    {
        end = steady_clock::now();
        diff = end - beg;
        printf("SDL Initialized! Initialization time: %f seconds!\n",diff.count());
    }

    inter = steady_clock::now();

    if(!renderer.initGL())
    {
        printf("Could not initialize OpenGL!\n");
        return false;
    }
    else
    {
        end = steady_clock::now();
        diff = end - inter;
        printf("Succesfully initialized OpenGL! Initialization time: %f seconds!\n", diff.count());
    }

    SDL_GL_SetSwapInterval(1);

    end = steady_clock::now();
    diff = end - beg;
    printf("Completed initialization! Total initialization time: %f seconds!\n", diff.count());

    return true;
}

void Engine::start(int lastFrame)
{
    while(active)
    {
    
        if(lastFrame == 0)
            lastFrame = SDL_GetTicks();
        
        //Lets process our events, first
        processEvents();

        //Because we're using a constant 
        //deltaT with a variable fps
        //we have no choice but to
        //do multiple physics steps if
        //our dF > dT (or else we could
        //potentially pass in really big
        //or really small values to the physics
        //simulation)
        int currentTime = SDL_GetTicks();
        //The change between frames
        int deltaF = currentTime - lastFrame;
        timeStepsToProcess += deltaF;


        //Sometimes we might have some timeSteps
        //left over to process, what with rendering
        //perhaps taking too long. This (hopefully)
        //mitigates that.
        while(timeStepsToProcess >= deltaT)
        {

            timeStepsToProcess -= deltaT;
            for(int i = 0; i < functions.size(); i++)
                functions[i](physT);

        }

        renderer.render(&activeScene);
        SDL_GL_SwapWindow(mainWindow);

        lastFrame = currentTime;
    }

    quit();

}

void Engine::processEvents()
{
    SDL_Event event;
    SDL_Keycode key;
    glm::vec3 lookat;

    /*while (SDL_PollEvent(&event)) 
    {
        switch(event.type)
        {
            //TODO: Send events to their rightful place
            break;
        }
    }*/
}

bool Engine::registerCallback( std::function<void(float)> function )
{
    /*if( std::find(functions.begin(), functions.end(), function) == functions.end())
    {*/
    functions.push_back( function );
    printf("Added callback!\n");

    return true;
    //}
    //return false;

}

bool Engine::initSDL()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return false;
    }

    //Mesa currently only supports a 3.3 core profile, AFAIK
    //So, being a Linux user with an Intel CPU (and mesa)
    //I'm going to have to stick with OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mainWindow = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_SetRelativeMouseMode( SDL_TRUE );

    if(!mainWindow)
    {
        printf("Woops, couldn't create the main window.");
        return false;
    }    
    printf("Done.\n");

    printf("Creating GL context... ");
    glContext = SDL_GL_CreateContext(mainWindow);
    printf("Done.\n");

    printf("Loading functions... ");

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        return false;
    }
    printf("Functions loaded!\n");
    printf("OpenGL version is %s\n", glGetString(GL_VERSION) );
    printf("GLSL version is %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return true;

}

void Engine::quit()
{
    printf("\nQuitting!\n");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    exit(0);
}

void Engine::setActiveScene( Scene toSet )
{
    activeScene = toSet;
}

Scene* Engine::getActiveScene()
{
    return &activeScene;
}

Engine::Engine()
{
}
