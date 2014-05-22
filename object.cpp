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

glm::mat4 Object::getTransform(void)
{
	glm::mat4 trans;
	trans = glm::translate( trans, position );
	trans = glm::rotate( trans, rotation[0], {1.0f,0.0f,0.0f} );
	trans = glm::rotate( trans, rotation[1], {0.0f,1.0f,0.0f} );
	trans = glm::rotate( trans, rotation[2], {0.0f,0.0f,1.0f} );
	trans = glm::scale( trans, scale );

	return trans;
}

void Object::translateBy(glm::vec3 vec)
{
	position += vec; 
}
