#include "object.h"

class DisplayObject : public Object 
{
    public: 

        DisplayObject( Model mod, Material* mat = NULL);
        //Lets just say we're using SI units
        float mass = 1.0f;

        Material getMaterial(size_t meshIndex = 0);
        void setMaterial(Material mat);

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

        bool customMatAvailable = false;

    private:

        Model model;

        glm::vec3 velocity;
        glm::vec3 rotVelocity;

        //By default, any object will
        //use the material stored in
        //its model. This can be 
        //overridden with the customMat
        //variable
        Material customMat;

};
