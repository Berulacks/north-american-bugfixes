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

void Object::rotateBy(glm::vec3 vec)
{
	rotation += vec; 
}


OOBB Object::getOOBB( int meshIndex )
{
	OOBB oobb = model->getBCombo( meshIndex ).ooBB;
	glm::mat4 transform = getTransform();

	glm::vec4 c2 = glm::vec4(oobb.center, 0.0f) * transform;

	glm::vec4 xAx = glm::vec4(oobb.axes[0], 0.0f) * transform;
	glm::vec4 yAx = glm::vec4(oobb.axes[1], 0.0f) * transform;
	glm::vec4 zAx = glm::vec4(oobb.axes[2], 0.0f) * transform;

	oobb.center = glm::vec3(c2);
	oobb.axes[0] = glm::vec3(xAx);
	oobb.axes[1] = glm::vec3(yAx);
	oobb.axes[2] = glm::vec3(zAx);

	return oobb;
}

//NEEDS TO TAKE A MESH INDEX WHEN WE ACTUALLY
//REMOVE ASSIMP AND START USING OUR OWN MESH
//CLASS. 
glm::vec3 Object::momentOfInertia()
{
	glm::vec3 mI;
	OOBB oobb = model->getBCombo( 0 ).ooBB;

	mI.x = ( oobb.e[0] * oobb.e[0] * 2 * 2 + oobb.e[2] * oobb.e[2] * 2 * 2 ) * mass * (1.0f / 12.0f);
	mI.y = ( oobb.e[1] * oobb.e[1] * 2 * 2 + oobb.e[2] * oobb.e[2] * 2 * 2 ) * mass * (1.0f / 12.0f);
	mI.z = ( oobb.e[1] * oobb.e[1] * 2 * 2 + oobb.e[0] * oobb.e[0] * 2 * 2 ) * mass * (1.0f / 12.0f);


	return mI;
}

void Object::addToVelocity(glm::vec3 toAdd)
{
	if( sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z) <= MAX_VEL)
		velocity += toAdd;

}


void Object::addToRotVelocity(glm::vec3 toAdd)
{
	if( sqrt(rotVelocity.x * rotVelocity.x + rotVelocity.y * rotVelocity.y + rotVelocity.z * rotVelocity.z) <= MAX_VEL)
		rotVelocity += toAdd;

}
