#include "object.h"

Material* Object::getMaterial()
{
	if( customMat != NULL )
		return customMat;
	return model->getMaterial();
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