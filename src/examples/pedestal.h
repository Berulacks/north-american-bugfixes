#include <nab.h>

class Pedestal
{

    Renderer* myRenderer;
    Storage* myStorage;
    Engine* myEngine;

    Object* obj;

    public:
        Pedestal(int argc, const char* argv[]);
        void processEvents(float physT);
};
