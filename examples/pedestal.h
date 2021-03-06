#include <nab.h>

class Pedestal
{

    Renderer* myRenderer;
    Storage* myStorage;
    Engine* myEngine;

    DisplayObject* obj;

    public:
        Pedestal(int argc, const char* argv[]);
        void processEvents(float physT);

    private:
        glm::vec3 cameraVel;
};
