#pragma once
#include "../renderer/renderer.h"

class Camera
{
        public:
                Camera( glm::vec3 startPos = glm::vec3(), glm::vec3 face = glm::vec3(0.0f,0.0f,1.0f) );

        private:

                glm::mat4 *transformation;


};
