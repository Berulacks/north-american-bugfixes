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

						//...we haven't already checked this pair.
						std::find(checked.begin(), checked.end(), pair) == checked.end() && \
				   
						//...there is a collision
						checkForCollision(objects[i]->getOOBB(0), objects[j]->getOOBB(0)) 
				  )
				{
					//collisionResolution(objects[i], objects[j]);
				}//end of if statement
			}//end of j for loop
		}//end of i for loop
	}//end of .size() if statement

	//Euler integration	
	for(int i = 0; i < objects.size(); i++)
	{
		if(objects[i]->isPhysicsObject)
		{

			objects[i]->addToVelocity( glm::vec3(0,-GRAVITY*deltaT,0) );
			objects[i]->translateBy( objects[i]->getVelocity() * deltaT);

			//printf("Object at: %f, %f, %f\n", objects[i]->getPosition().x, objects[i]->getPosition().y, objects[i]->getPosition().z);

			objects[i]->rotateBy( objects[i]->getRotVelocity() * deltaT );

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
		if (abs(t[i]) > ra + rb) 
		{
			printf("AIN'T GOT NO COLLISION\n");
			return false;
		}
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
	printf("COLLISION!\n");
	return true;
}


void Physics::collisionResolution(Object* ob1, Object* ob2)
{
	//printf("Resolving collisions!\n");
	//Initialized to {0,0,0}
	glm::vec3 vAP, vBP;

	if(ob1->isPhysicsObject)
		glm::vec3 vAP = ob1->getVelocity();
	if(ob2->isPhysicsObject)
		glm::vec3 vBP = ob2->getVelocity();

	glm::vec3 vAB = vAP - vBP;

	OOBB oobb1 = ob1->getOOBB(0);
	OOBB oobb2 = ob2->getOOBB(0);

	glm::vec3 cAB = oobb1.center - oobb2.center;
	//We have seperate points for the collision,
	//p1 (on object 1), and p2 (on object2).
	//This means that the collision resolution isn't
	//100% accurate, but it works
	glm::vec3 P1, P2, rAP, rBP;
	
	//Normal vector from ob1.center to ob2.center
	//THIS NEEDS TO BE FIXED LATER, THIS IS A GUESS
	//AND WILL PROBABLY RESULT IN WONKY,
	//IF NOT OUTRIGHT BROKEN COLLISIONS
	glm::vec3 N = glm::normalize( cAB );

	P1 = getPointOnOOB( oobb1, cAB / 2.0f );
	P2 = getPointOnOOB( oobb2, cAB / 2.0f );


	float j = - (1.0f + E_CONSTANT) * glm::dot(vAB, N)
		/ 
		  glm::dot( 
		    ( (1/ob1->mass + 1/ob2->mass)*N 
		      + 
		    ( glm::cross( 
		    ( glm::cross(rAP,N) / ob1->momentOfInertia() ) ,
		      rAP) ) 
		      +
		    glm::cross( (glm::cross(rBP, N) / ob2->momentOfInertia() ),
			    rBP)
		  ) , N);

	ob1->addToVelocity( ob1->getVelocity() + ( j / ob1->mass ) * N );
	ob2->addToVelocity( ob2->getVelocity() + ( -j / ob2->mass ) * N );

	//ob1->rotVelocity = ob1->rotVelocity + ( glm::dot( rAP, j * N ) / ob1->momentOfInertia() );
	//ob2->rotVelocity = ob2->rotVelocity + ( glm::dot( rBP, -j * N ) / ob2->momentOfInertia() );
	ob1->addToVelocity( ob1->getRotVelocity() + ( glm::cross( rAP, j * N ) / ob1->momentOfInertia() ));
	ob2->addToVelocity( ob2->getRotVelocity() + ( glm::cross( rBP, -j * N ) / ob2->momentOfInertia() ));
}

glm::vec3 Physics::getPointOnOOB( OOBB ob1, glm::vec3 point )
{
	glm::vec3 d = point - ob1.center;
	// Start result at center of box; make steps from there
	glm::vec3 q = ob1.center;
	
	// For each OBB axis...
	for (int i = 0; i < 3; i++) 
	{
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		float dist = glm::dot(d, ob1.axes[i]);
		// If distance farther than the box extents, clamp to the box
		if (dist > ob1.e[i]) dist = ob1.e[i];
		if (dist < -ob1.e[i]) dist = -ob1.e[i];
		// Step that distance along the axis to get world coordinate
		q += dist * ob1.axes[i];
	}


	return q;
}
