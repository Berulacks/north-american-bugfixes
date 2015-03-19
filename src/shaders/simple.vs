#version 330
//Most valuable player
uniform mat4 mvp;

//incoming vector
in vec3 theV;

void main()
{
    gl_Position = mvp * vec4(theV,1.0);
}

