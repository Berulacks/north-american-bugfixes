#include "model.h"

/*#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>*/

//Objects are the actual in-engine objects
//Each object has a unique transformation matrix
//But can use any of the models or materials
//loaded in the storage class
class Object
{
	public:
		Object();
		Model* getModel(void) { return model; };
		glm::mat4 getTransform(void);
		Material* getMaterial(void);

	private:
		Model* model;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		//By default, any object will
		//use the material stored in
		//its model. This can be 
		//overridden with the customMat
		//variable, which is NULL by
		//default.
		Material* customMat = NULL;

};
