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
						checkForCollision(objects[i]->getModel()->getBCombo(0).ooBB, objects[j]->getModel()->getBCombo(0).ooBB) 
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
	return true;
}


void collisionResolution(Object* ob1, Object* ob2)
{
	printf("Resolving collisions!\n");
}
