#pragma once
#include "model.h"

#define MAX_VEL 10.0f

//Objects are the actual in-engine objects
//Each object has a unique transformation matrix
//But can use any of the models or materials
//loaded in the storage class
class Object
{
	public:
		//Lets just say we're using SI units
		float mass = 1.0f;

		Object(Model* mod, Material* mat = NULL);
		Model* getModel(void) { return model; };
		glm::mat4 getTransform(void);
		Material* getMaterial(void);

		void translateBy(glm::vec3);
		void rotateBy(glm::vec3);
		void setScale(glm::vec3 target){ scale = target; };

		//Should we apply physics steps to this object?
		bool isPhysicsObject = false;
		//Should we check if this object collides with others?
		bool collidable = false;
		//Should we NOT render this object?
		bool hidden = false;

		bool renderBoundingBox = false;

		glm::vec3 momentOfInertia(void);

		glm::vec3 getVelocity(void) { return velocity; };
		glm::vec3 getRotVelocity(void) { return rotVelocity; };

		void addToVelocity(glm::vec3 toAdd);
		void addToRotVelocity(glm::vec3 toAdd);

		glm::vec3 getPosition() { return position; };

		OOBB getOOBB( int meshIndex );

	private:
		Model* model;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

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
