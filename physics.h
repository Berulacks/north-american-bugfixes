#include "object.h"

#define GRAVITY 9.8

class Physics
{
	public:
		Physics() {};
		void updatePhysics( std::vector<Object*> objects, float deltaT );
		bool checkForCollision(OOBB ob1, OOBB ob2);
		void collisionResolution(Object* ob1, Object* ob2);
		glm::vec3 getPointOnOOB(OOBB ob1, glm::vec3 point);
};
