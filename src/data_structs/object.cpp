#include "object.h"


Object::Object(Model mod, Material* mat)
{
    model = mod;
    if(mat != NULL)
        customMat = mat;
    scale = {1,1,1};
}

Material* Object::getMaterial()
{
    return customMat;
}

void Object::setTransform( glm::mat4 trans )
{
    /*printf("Our current position, rotation, and scale values are: \n");
    printf("Position: %f, %f, %f\n", position[0], position[1], position[2] );
    printf("Rotation: %f, %f, %f, %f\n", rotation[0], rotation[1], rotation[2], rotation[3] );
    printf("Scale: %f, %f, %f\n", scale[0], scale[1], scale[2] );*/

    glm::vec3 tempVec;

    //Step 1: Find position
    tempVec = glm::vec3(  trans[3][0], trans[3][1], trans[3][2] );
    //printf("\nOur decomposed position is: %f, %f, %f\n", tempVec[0], tempVec[1], tempVec[2]);

    //WARNING: This could cause issues with negative scale values. Should be investigated. Or perhaps this function shouldn't exist in the first place?
    //Step 2: Find scale
    glm::vec3 tempScale;

    tempScale[0] = glm::length( glm::vec3( trans[0][0], trans[0][1], trans[0][2] ) );
    tempScale[1] = glm::length( glm::vec3( trans[1][0], trans[1][1], trans[1][2] ) );
    tempScale[2] = glm::length( glm::vec3( trans[2][0], trans[2][1], trans[2][2] ) );

    //printf("Our decomposed scale is: %f, %f, %f\n", tempScale[0], tempScale[1], tempScale[2]);


    //Step 3: Find Rotation
    //trace(trans) = 1 + 2cos(theta)
    //(we need to scale down all elements on the diagonal, if the scale is more than 1.
    float angle;
    float scaleMag = glm::length( tempScale );

    if( scaleMag > 1 )
        angle = acos( ( ((trans[0][0] / tempScale[0]) + (trans[1][1] / tempScale[1]) + (trans[2][2] / tempScale[2]) ) - 1 ) * 0.5 );
    else
        angle = acos( ( ((trans[0][0]) + (trans[1][1]) + (trans[2][2]) ) - 1 ) * 0.5 );
    
    //printf("Angle is %f.\n", angle);

    glm::quat q;

    double xr = 0;
    double yr = 0;
    double zr = 0;

    if(angle == 0)
    {
        q[3] = 1;
        q[0] = 0;
        q[1] = 0;
        q[2] = 0;
    }
    else if(angle < M_PI)
    {
        //Extract axises from skew symmetric matrix
        xr = (trans[1][2] - trans[2][1])/(2*sin(angle));
        yr = (trans[2][0] - trans[0][2])/(2*sin(angle));
        zr = (trans[0][1] - trans[1][0])/(2*sin(angle));
    }
    else if(angle == M_PI)
    {

        if( scaleMag > 1 )
            xr = 0.5*sqrt((trans[0][0] / tempScale[0])-(trans[1][1] / tempScale[1])-(trans[2][2] / tempScale[2])+1);
        else
            xr = 0.5*sqrt((trans[0][0])-(trans[1][1])-(trans[2][2])+1);

        yr = trans[1][0]/(2*xr);
        zr = trans[2][0]/(2*xr);
    }

    q[3] = cos(angle/2);
    q[0] = xr * sin(angle*0.5);
    q[1] = yr * sin(angle*0.5);
    q[2] = zr * sin(angle*0.5);

    q = glm::normalize( q );

    //printf("Our decomposed rotation: x:%f, y:%f, z:%f, w:%f\n", q.x, q.y, q.z, q.w );


    this->position = tempVec;
    this->rotation = q;
    this->scale = tempScale;
}

glm::mat4 Object::getTransform(void)
{
    glm::mat4 trans;

    trans = glm::translate( trans, position );
    trans *= glm::mat4_cast( rotation );
    trans = glm::scale( trans, scale );

    return trans;
}

glm::vec3 Object::getPosition()
{
    return position;
}

glm::vec3 Object::getScale()
{
    return scale;
}

void Object::translateBy(glm::vec3 vec)
{
    position += vec; 
}

void Object::rotateBy(glm::vec3 eulerAngles)
{
    //printf("\nStarting rotate. Initial rotation is %f, %f, %f, %f.\n", rotation[0], rotation[1], rotation[2], rotation[3]);
    
    glm::quat interimQ;

    //Rotate by x
    if( eulerAngles[0] != 0 )
    {
        interimQ = glm::quat( cos( eulerAngles.x / 2 ), sin( eulerAngles.x * 0.5f ), 0, 0 );
        glm::normalize( interimQ );
        rotation *= interimQ;
    }
    //Rotate by y
    if( eulerAngles[1] != 0 )
    {
        interimQ = glm::quat( cos( eulerAngles.y / 2 ), 0, sin( eulerAngles.y * 0.5f ), 0 );
        glm::normalize( interimQ );
        rotation *= interimQ;
    }
    //Rotate by z
    if( eulerAngles[2] != 0 )
    {
        interimQ = glm::quat( cos( eulerAngles.z / 2 ), 0, 0, sin( eulerAngles.z * 0.5f ));
        glm::normalize( interimQ );
        rotation *= interimQ;
    }
    rotation = glm::normalize( rotation );
    //printf("Finishing rotate. Final rotation is %f, %f, %f, %f.\n\n", rotation[0], rotation[1], rotation[2], rotation[3]);
}
