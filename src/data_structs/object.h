#include "model.h"

//Objects are the actual in-engine objects
//Each object has a unique transformation matrix
//But can use any of the models or materials
//loaded in the storage class
class Object
{
    public:

        Object();
        glm::mat4 getTransform(void);

        void translateBy(glm::vec3);
        void rotateBy(glm::vec3 eulerAngles);

        void setTranslation(glm::vec3 translation);
        void setScale(glm::vec3 target);
        void setRotation(glm::quat target);
        void setTransform( glm::mat4 trans );

        glm::vec3 getPosition(void);
        glm::vec3 getScale(void);

    private:

        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        //Should we use a static transform
        //instead of recalculating every frame?
        bool useTransform = false;
        glm::mat4* transform;

        //Sets useTransform to false and wipes
        //the pointer to the transformation matrix
        //Should only be called by functions that
        //affect position, rotation, or scale.
        void clearTransform(void);
};
