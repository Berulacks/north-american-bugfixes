#include "bullet_demo.h"

void BulletDemo::processEvents(float physT)
{
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
					myRenderer->cameraPos -= glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_s)
					myRenderer->cameraPos += glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_a)
					myRenderer->cameraPos -= glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_d)
					myRenderer->cameraPos += glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(myRenderer->camera) * physT;

				if(key == SDLK_f)
					myRenderer->toggleFullScreen(myEngine->getWindow() );

                if(key == SDLK_o)
                    addObj( mod, false, 1, findCameraPoint() );
                if(key == SDLK_p)
                    addObj( cube, true, 1, findCameraPoint() );

				break;

			case SDL_MOUSEMOTION:

				myRenderer->xRot -= event.motion.xrel * 0.001;
				myRenderer->yRot -= event.motion.yrel * 0.001;
				break;
            case SDL_MOUSEBUTTONUP:
                fireObj();
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

glm::vec3 BulletDemo::findCameraPoint()
{
    glm::vec3 lookat;
	lookat.x = sinf(myRenderer->xRot) * cosf(myRenderer->yRot);
	lookat.y = sinf(myRenderer->yRot);
	lookat.z = cosf(myRenderer->xRot) * cosf(myRenderer->yRot);

    return myRenderer->cameraPos + lookat * 5.0f;
}

void BulletDemo::fireObj(void)
{
    std::pair<Object*,btRigidBody*> pair = addObj( mod, false, 2, myRenderer->cameraPos );
    btRigidBody* body = pair.second;

    glm::vec3 velocity = (findCameraPoint() - myRenderer->cameraPos) * 3.0f;

    body->setLinearVelocity( btVector3( velocity.x, velocity.y, velocity.z ) );
}


void BulletDemo::initBullet(void)
{
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -5, 0));


    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

std::pair<Object*,btRigidBody*> BulletDemo::addObj(Model model, bool isCube, float radius, glm::vec3 position)
{


    btCollisionShape* fallShape;
    if(!isCube) 
        fallShape = new btSphereShape(radius);
    else
        fallShape = new btBoxShape( btVector3(1,1,1) );

    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z) ) );

    btScalar mass = 0.2;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* body = new btRigidBody(fallRigidBodyCI);
    bodies.push_back( body );
    dynamicsWorld->addRigidBody(body);


	DisplayObject* obj = new DisplayObject(model);

    if(isCube && renderBBox)
        obj->renderBoundingBox = true;
	obj->translateBy( {position.x,position.y,position.z} );

	myEngine->getActiveScene()->registerObject( obj );
    objs.push_back( obj );

    return std::pair<Object*,btRigidBody*>(obj, body);
}

void BulletDemo::step(float physT)
{

    dynamicsWorld->stepSimulation(physT, 8);
    btTransform trans;
    glm::mat4 finalT;

    for(int i = 0; i < objs.size(); i++)
    {

        bodies[i]->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix( glm::value_ptr( finalT ) );

        objs[i]->setTransform( finalT );
    }
}

BulletDemo::BulletDemo( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init("Bullet Demo", argc, argv) )
		exit(1);

    myEngine = &program;

	printf("Program initialized, let's add a callback!\n");

	program.registerCallback( std::bind( &BulletDemo::processEvents, this, std::placeholders::_1 ) );
    program.registerCallback(std::bind( &BulletDemo::step, this, std::placeholders::_1 ));
	
	myStorage = program.getStorage();
    myStorage->addTextureFolder( "./examples/textures/" );

	myRenderer = program.getRenderer();
    myRenderer->cameraPos = {0.0, 3.0, 0.0};

	std::vector<const char*> files;

    renderBBox = false;

	if(argv[1] == NULL)
		files.push_back( "./examples/models/cube/cube.obj");
	else
    {
		files.push_back( argv[1] );
        renderBBox = true;
    }

	if( !myStorage->readModel(files[0]) 
            ||  
        !myStorage->readModel( "./examples/models/sphere/sphere.obj" ) )
    {
        printf("Could not load models!\n");
		program.quit();
    }
	
    mod =  myStorage->loadModel( "./examples/models/sphere/sphere.obj" );
    cube =  myStorage->loadModel( files[0] );

	/*printf("Okay, our model is supposedly loaded, lets check it for some info:\n");
	printf("Our model has %i meshes.\n", mod.numMeshes() );
	printf("The first mesh of our model is called %s\n", mod.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", mod.materials[0].name.c_str());
	printf("And it looks for a texture called %s\n", mod.materials[0].texDiffuse_name.c_str() );*/

    initBullet();

    if( !myStorage->readModel( "./examples/models/plane/plane.obj" ) )
        program.quit();

    DisplayObject plane = DisplayObject( myStorage->loadModel( "./examples/models/plane/plane.obj" ) );

    plane.setTranslation( {0, 0, 10} );
    glm::vec3 scale(2.0f,2.0f,1.0f);
    plane.setScale( scale );

    program.getActiveScene()->registerObject( &plane );

	program.start( SDL_GetTicks() );
	
}


int main( int argc, const char* argv[] )
{
    BulletDemo pedestal(argc, argv);
    return 0;
}
