#include "object.h"


Object::Object(Model* mod, Material* mat)
{
	model = mod;
	if(mat != NULL)
		customMat = mat;
	scale = {1,1,1};
}

Material* Object::getMaterial()
{
	return customMat;
}

void Object::setTransform( glm::mat4 trans )
{
    transform = trans;
    useTransform = true;
}
glm::mat4 Object::getTransform(void)
{
	glm::mat4 trans;

    if(!useTransform)
    {
        trans = glm::translate( trans, position );
        trans = glm::rotate( trans, rotation[0], {1.0f,0.0f,0.0f} );
        trans = glm::rotate( trans, rotation[1], {0.0f,1.0f,0.0f} );
        trans = glm::rotate( trans, rotation[2], {0.0f,0.0f,1.0f} );
        trans = glm::scale( trans, scale );

        return trans;
    }
    else
        return transform;
}

glm::vec3 Object::getPosition()
{
    //TODO: Fix this
    /*if(useTransform)
    {
        return glm::vec3( transform[12], transform[13], transform[14] );
    }*/
    return position;
}

void Object::translateBy(glm::vec3 vec)
{
	position += vec; 
}
