#pragma once

#include "object.h"

#define GRAVITY 9.8f
//For collision detection!
#define EPSILON 0.00001f
#define E_CONSTANT 1.0f

class Physics
{
	public:
		Physics() {};
		static void updatePhysics( std::vector<Object*> objects, float deltaT );
		static bool checkForCollision(OOBB ob1, OOBB ob2);
		static void collisionResolution(Object* ob1, Object* ob2);
		static glm::vec3 getPointOnOOB(OOBB ob1, glm::vec3 point);
};
