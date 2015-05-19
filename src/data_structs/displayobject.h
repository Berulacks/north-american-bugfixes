#include "object.h"

class DisplayObject : public Object 
{
    public: 

        DisplayObject( Model mod, Material* mat = NULL);
        //Lets just say we're using SI units
        float mass = 1.0f;

        Material* getMaterial(void);
        Model getModel(void) { return model; };
        //Should we apply physics steps to this object?
        bool isPhysicsObject = false;
        //Should we check if this object collides with others?
        bool collidable = false;
        //Should we NOT render this object?
        bool hidden = false;

        bool renderBoundingBox = false;

        //TODO: After OOB generation
        float momentOfInertia();

    private:

        Model model;

        glm::vec3 velocity;
        glm::vec3 rotVelocity;

        //By default, any object will
        //use the material stored in
        //its model. This can be 
        //overridden with the customMat
        //variable, which is NULL by
        //default.
        Material* customMat = NULL;

};
