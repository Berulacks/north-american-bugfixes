#include "engine.h"
#include <BulletDynamics/btBulletDynamicsCommon.h>

class Pedestal
{

    Renderer* myRenderer;
    Storage* myStorage;
    Engine* myEngine;

    //Our sphere (in-engine representation)
    std::vector< Object* > objs;
    std::vector< btRigidBody* > bodies;

    btDiscreteDynamicsWorld* dynamicsWorld;
    //Our sphere (bullet representation)
    btRigidBody* fallRigidBody;


    public:
        Pedestal(int argc, const char* argv[]);
        void processEvents(float physT);
        void initBullet(void);
        void step(float physT);


};
