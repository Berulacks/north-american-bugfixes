#include "camera.h"

Camera::Camera( glm::vec3 startPos, glm::vec3 face )
{
    position = startPos;
}

bool Camera::operator== ( const Camera &toCompare )
{
    glm::mat4 mine = getTransform();
    glm::mat4 yours = toCompare.calculateTransform();

    if( mine != yours )
        return false;

    return true;

}
