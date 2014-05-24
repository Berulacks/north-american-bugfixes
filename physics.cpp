#include "physics.h"

void Physics::updatePhysics( std::vector<Object *> objects, float deltaT )
{
	//Collisions
	if(objects.size() >= 1)
	{
		std::vector< std::pair< Object*, Object* > > checked;
		for(int i = 0; i < objects.size(); i++)
		{
			for(int j = 0; j < objects.size(); j++)
			{
				if(!objects[j]->collidable)
					continue;

				std::pair<Object*,Object*> pair(objects[i], objects[j]);

				if( \
						//If...
						//...we're not trying to compare to
						//the object we're checking for
						objects[j] != objects[i] && \

						//...our object is flagged for 
						//collisions
						objects[j]->collidable && \
						
						//...we haven't already checked this pair.
						std::find(checked.begin(), checked.end(), pair) != checked.end() && \
				   
						//...there is a collision
						checkForCollision(objects[i]->getOOBB(0), objects[j]->getOOBB(0)) 
				  )
				{
					collisionResolution(objects[i], objects[j]);
				}//end of if statement
			}//end of j for loop
		}//end of i for loop
	}//end of .size() if statement

	//Euler integration	
	for(int i = 0; i < objects.size(); i++)
	{
		if(objects[i]->isPhysicsObject)
		{

			objects[i]->velocity += GRAVITY * deltaT;
			objects[i]->translateBy( objects[i]->velocity );

			objects[i]->rotateBy( objects[i]->rotVelocity );

		}//end of isPhysicsObject if statement
	}//end of i for loop
}

bool Physics::checkForCollision( OOBB ob1, OOBB ob2)
{
	printf("Checking for collisions!\n");
	float ra, rb;
	glm::mat3 R, AbsR;
	// Compute rotation matrix expressing b in a’s coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(ob1.axes[i], ob2.axes[j]);
	
	// Compute translation vector t
	glm::vec3 t = ob2.center - ob1.center;
	// Bring translation into a’s coordinate frame
	t = glm::vec3(glm::dot(t, ob1.axes[0]), glm::dot(t, ob1.axes[2]), glm::dot(t, ob1.axes[2]));
	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR[i][j] = abs(R[i][j]) + EPSILON;
	
	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = ob1.e[i];
		rb = ob2.e[0] * AbsR[i][0] + ob2.e[1] * AbsR[i][1] + ob2.e[2] * AbsR[i][2];
		if (abs(t[i]) > ra + rb) return false;
	}
	
	// Test e L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = ob1.e[0] * AbsR[0][i] + ob1.e[1] * AbsR[1][i] + ob1.e[2] * AbsR[2][i];
		rb = ob2.e[i];
		if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}
	// Test axis L = A0 x B0
	ra = ob1.e[1] * AbsR[2][0] + ob1.e[2] * AbsR[1][0];
	rb = ob2.e[1] * AbsR[0][2] + ob2.e[2] * AbsR[0][1];
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;
	
	// Test axis L = A0 x B1
	ra = ob1.e[1] * AbsR[2][1] + ob1.e[2] * AbsR[1][1];
	rb = ob2.e[0] * AbsR[0][2] + ob2.e[2] * AbsR[0][0];
	if (abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;

	// Test axis L = A0 x B2
	ra = ob1.e[1] * AbsR[2][2] + ob1.e[2] * AbsR[1][2];
	rb = ob2.e[0] * AbsR[0][1] + ob2.e[1] * AbsR[0][0];
	if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

	// Test axis L = A1 x B0
	ra = ob1.e[0] * AbsR[2][0] + ob1.e[2] * AbsR[0][0];
	rb = ob2.e[1] * AbsR[1][2] + ob2.e[2] * AbsR[1][1];
	if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;
	
	// Test axis L = A1 x B1
	ra = ob1.e[0] * AbsR[2][1] + ob1.e[2] * AbsR[0][1];
	rb = ob2.e[0] * AbsR[1][2] + ob2.e[2] * AbsR[1][0];
	if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;
	// Test axis L = A1 x B2
	ra = ob1.e[0] * AbsR[2][2] + ob1.e[2] * AbsR[0][2];
	rb = ob2.e[0] * AbsR[1][1] + ob2.e[1] * AbsR[1][0];
	if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;
	// Test axis L = A2 x B0
	ra = ob1.e[0] * AbsR[1][0] + ob1.e[1] * AbsR[0][0];
	rb = ob2.e[1] * AbsR[2][2] + ob2.e[2] * AbsR[2][1];
	if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;
	// Test axis L = A2 x B1
	ra = ob1.e[0] * AbsR[1][1] + ob1.e[1] * AbsR[0][1];
	rb = ob2.e[0] * AbsR[2][2] + ob2.e[2] * AbsR[2][0];
	if 
	(abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;
	// Test axis L = A2 x B2
	ra = ob1.e[0] * AbsR[1][2] + ob1.e[1] * AbsR[0][2];
	rb = ob2.e[0] * AbsR[2][1] + ob2.e[1] * AbsR[2][0];
	if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;
	
	// Since no separating axis is found, the OBBs must be intersecting
	return true;
}


void collisionResolution(Object* ob1, Object* ob2)
{
	printf("Resolving collisions!\n");
}
