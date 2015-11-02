#include "pedestal.h"

void Pedestal::processEvents(float physT)
{
    myRenderer->cameraPos += cameraVel * glm::mat3(myRenderer->camera) * physT;

    if( abs( cameraVel.length() ) <= 0.05 && cameraVel.length() != 0.0f)
        cameraVel = {0,0,0};
    else
        cameraVel *= 0.8f;

    SDL_Event event;
    SDL_Keycode key;
    glm::vec3 lookat;

    while (SDL_PollEvent(&event)) 
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:

                key = event.key.keysym.sym;

                if(key == SDLK_SPACE
                        ||
                    key == SDLK_RETURN
                        ||
                    key == SDLK_ESCAPE)

                    myEngine->quit();

                if(key == SDLK_w)
                    cameraVel -= glm::vec3(0.0f, 0.0f, 3.0f);

                if(key == SDLK_s)
                    cameraVel += glm::vec3(0.0f, 0.0f, 3.0f);

                if(key == SDLK_a)
                    cameraVel -= glm::vec3(3.0f, 0.0f, 0.0f);

                if(key == SDLK_d)
                    cameraVel += glm::vec3(3.0f, 0.0f, 0.0f);

                if(key == SDLK_f)
                    myRenderer->toggleFullScreen(myEngine->getWindow() );
                if(key == SDLK_b)
                    obj->renderBoundingBox = !obj->renderBoundingBox ;
                if(key == SDLK_UP)
                    obj->rotateBy( {0.1f, 0.0f, 0.0f} );
                if(key == SDLK_DOWN)
                    obj->rotateBy( {-0.1, 0.0f, 0.0f} );
                if(key == SDLK_RIGHT)
                    obj->rotateBy( {0.0f, 0.1f, 0.0f} );
                if(key == SDLK_LEFT)
                    obj->rotateBy( {0.0f, -0.1f, 0.0f} );

                break;

            case SDL_MOUSEMOTION:

                myRenderer->xRot -= event.motion.xrel * 0.001;
                myRenderer->yRot -= event.motion.yrel * 0.001;

                break;
            
            case SDL_MOUSEWHEEL:

                obj->setScale( ( obj->getScale() += 0.03 * event.wheel.y ) );
                
                break;
        }

    }

    if(myRenderer->xRot < -M_PI)
        myRenderer->xRot += M_PI * 2;

    else if(myRenderer->xRot > M_PI)
        myRenderer->xRot -= M_PI * 2;

    if(myRenderer->yRot < -M_PI / 2)
        myRenderer->yRot = -M_PI / 2;

    if(myRenderer->yRot > M_PI / 2)
        myRenderer->yRot = M_PI / 2;

    lookat.x = sinf(myRenderer->xRot) * cosf(myRenderer->yRot);
    lookat.y = sinf(myRenderer->yRot);
    lookat.z = cosf(myRenderer->xRot) * cosf(myRenderer->yRot);

    myRenderer->camera = glm::lookAt(myRenderer->cameraPos, myRenderer->cameraPos + lookat, glm::vec3(0, 1, 0));
}



Pedestal::Pedestal( int argc, const char* argv[] )
{
    Engine program;
    if( !program.init("Pedestal", argc, argv) )
        exit(1);

    myEngine = &program;

    printf("Program initialized, let's add a callback!\n");

    using std::placeholders::_1;
    program.registerCallback( std::bind( &Pedestal::processEvents, this, _1 ) );
    
    myStorage = program.getStorage();
    myRenderer = program.getRenderer();

    myStorage->addTextureFolder( "./examples/textures/" );

    myRenderer->cameraPos = {0.0, 3.0, 0.0};

    std::vector<const char*> files;

    if(argv[1] == NULL)
        files.push_back( "./examples/models/suzanne.obj");
    else
    {
        files.push_back( argv[1] );
    }

    if( !myStorage->readModel(files[0]) )
    {
        printf("Could not load model ' %s '!\n",files[0]);
        program.quit();
    }
    
    Model model = myStorage->loadModel( files[0] );

    printf("Okay, our modelel is supposedly loaded, lets check it for some info:\n");
    printf("Our modelel has %i meshes.\n", model.numMeshes() );
    printf("The first mesh of our modelel is called %s\n", model.getMeshInfo(0).name.c_str());
    printf("...and its material is called %s\n", model.materials[0].name.c_str());
    printf("And it looks for a texture called %s\n", model.materials[0].texDiffuse_name.c_str() );

    if( !myStorage->readModel( "./examples/models/plane/plane.obj" ) )
        program.quit();

    DisplayObject plane = DisplayObject( myStorage->loadModel( "./examples/models/plane/plane.obj" ) );
    plane.setTranslation( {0, -2, 4} );
    program.getActiveScene()->registerObject( &plane );

    obj = new DisplayObject(model);
    obj->translateBy( {0.0f,0.0f,5.0f} );
    myEngine->getActiveScene()->registerObject( obj );

    Program blurShader = Program( "./src/shaders/vertex.vs", "./src/shaders/blur.fs" );
    /*if(blurShader.isReady())
    {
        Material h = obj->getMaterial(0);
        h.shader = &blurShader;
        obj->setMaterial( h );
    }*/

    program.start( SDL_GetTicks() );
    
}


int main( int argc, const char* argv[] )
{
    Pedestal pedestal(argc, argv);
    return 0;
}
