#include "engine.h"
#include <BulletDynamics/btBulletDynamicsCommon.h>

Renderer* myRenderer;
Storage* myStorage;
Engine* myEngine;

//Our sphere (in-engine representation)
Object* sphere;

btDiscreteDynamicsWorld* dynamicsWorld;
//Our sphere (bullet representation)
btRigidBody* fallRigidBody;

void processEvents(float physT)
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

				break;

			case SDL_MOUSEMOTION:

				myRenderer->xRot -= event.motion.xrel * 0.001;
				myRenderer->yRot -= event.motion.yrel * 0.001;
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

void initBullet(void)
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


    btCollisionShape* fallShape = new btSphereShape(2);
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 15)));

    btScalar mass = 0.2;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    fallRigidBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(fallRigidBody);

}

void step(float physT)
{

    dynamicsWorld->stepSimulation(physT, 8);
    btTransform trans;
    fallRigidBody->getMotionState()->getWorldTransform(trans);

    GLfloat* matrix = new GLfloat[15];
    trans.getOpenGLMatrix( matrix );

    sphere->setTranslation( { matrix[12], matrix[13], matrix[14] } );
    printf("Trans is %f, %f, %f!\n", matrix[12], matrix[13], matrix[14] );

}

int main( int argc, const char* argv[] )
{
	Engine program;
	if( !program.init(argc, argv) )
		exit(1);

    myEngine = &program;

	printf("Program initialized, let's add a callback!\n");

	program.registerCallback( processEvents );
    program.registerCallback( step );
	
	myStorage = program.getStorage();
	myRenderer = program.getRenderer();

    myRenderer->cameraPos = {0.0, 3.0, 0.0};

	std::vector<const char*> files;
	if(argv[1] == NULL)
		files.push_back( "./models/suzanne.obj");
	else
		files.push_back( argv[1] );

	if( !myStorage->readModel(files[0]) )
		program.quit();
	
	Model mod = *( myStorage->loadModel( files[0] ) );
	sphere = new Object(&mod);
	sphere->translateBy( {0.0f,0.0f,5.0f} );
	program.registerObject( sphere );

	printf("Okay, our model is supposedly loaded, lets check it for some info:\n");
	printf("Our model has %i meshes.\n", mod.numMeshes() );
	printf("The first mesh of our model is called %s\n", mod.getBCombo(0).name.c_str());
	printf("...and its material is called %s\n", mod.materials[0].name.c_str());
	printf("And it looks for a texture called %s\n", mod.materials[0].texDiffuse_name.c_str() );

    initBullet();

    if( !myStorage->readModel( "./models/plane/plane.obj" ) )
        program.quit();

    Object plane = Object( myStorage->loadModel( "./models/plane/plane.obj" ) );
    plane.setTranslation( {0, 1, 10} );
    program.registerObject( &plane );


	program.start( SDL_GetTicks() );

	return 0;
	
}
