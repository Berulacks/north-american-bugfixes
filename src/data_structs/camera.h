#pragma once
#include "displayobject.h"

class Camera : Object
{
        public:
                Camera( glm::vec3 startPos = glm::vec3(), glm::vec3 face = glm::vec3(0.0f,0.0f,1.0f) );
                bool operator== ( const Camera &toCompare );

};
