#include "engine.h"
#include <BulletDynamics/btBulletDynamicsCommon.h>

class Pedestal
{

    Renderer* myRenderer;
    Storage* myStorage;
    Engine* myEngine;

    //Model chosen by user
    Model mod;
    Model cube;

    //Our sphere (in-engine representation)
    std::vector< Object* > objs;
    std::vector< btRigidBody* > bodies;

    btDiscreteDynamicsWorld* dynamicsWorld;

    public:
        Pedestal(int argc, const char* argv[]);
        void processEvents(float physT);
        void initBullet(void);
        void step(float physT);

        void addObj(Model* model, bool isCube, float radius=2,glm::vec3 position={0,2,5});

        glm::vec3 findCameraPoint(void);


};
