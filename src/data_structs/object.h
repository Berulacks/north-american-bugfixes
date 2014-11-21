#include "model.h"

//Objects are the actual in-engine objects
//Each object has a unique transformation matrix
//But can use any of the models or materials
//loaded in the storage class
class Object
{
	public:
        //Lets just say we're using SI units
        float mass = 1.0f;

        Object() { scale = {1,1,1}; };
        Object(Model* mod, Material* mat = NULL);
        Model* getModel(void) { return model; };
        glm::mat4 getTransform(void);
        Material* getMaterial(void);

        void translateBy(glm::vec3);
        void setTranslation(glm::vec3 translation) { position = translation; };
        void setScale(glm::vec3 target){ scale = target; };
        void setTransform( glm::mat4 trans );

        glm::vec3 getPosition(void);

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

        Model* model;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::vec3 velocity;
        glm::vec3 rotVelocity;

        //Should we use a static transform
        //instead of recalculating every frame?
        bool useTransform = false;
        glm::mat4 transform;

        //By default, any object will
        //use the material stored in
        //its model. This can be 
        //overridden with the customMat
        //variable, which is NULL by
        //default.
        Material* customMat = NULL;

};
