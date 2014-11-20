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
    hasTransform = true;
}
glm::mat4 Object::getTransform(void)
{
	glm::mat4 trans;

    if(!hasTransform)
    {
        trans = glm::translate( trans, position );
        trans = glm::rotate( trans, rotation[0], {1.0f,0.0f,0.0f} );
        trans = glm::rotate( trans, rotation[1], {0.0f,1.0f,0.0f} );
        trans = glm::rotate( trans, rotation[2], {0.0f,0.0f,1.0f} );
        trans = glm::scale( trans, scale );
    }
    else
        return transform;

	return trans;
}

void Object::translateBy(glm::vec3 vec)
{
	position += vec; 
}
