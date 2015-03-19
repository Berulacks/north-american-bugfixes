#version 330
//Most valuable player
uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normal_matrix;
uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ld; // Light source intensity

//outgoing color
out vec4 col;
out vec2 tex;

//incoming vector
in vec3 theV;
//incoming normal
in vec3 theN;
//incoming texture coord
in vec2 tex_in;

void main()
{

    //col = 10.0 * vec4(theV, 0);
    tex = tex_in;

    // Convert normal and position to eye coords
    vec3 tnorm = normalize( normal_matrix * theN );
    //vec3 tnorm = normalize( toNorm );
    vec4 eyeCoords = mv * vec4(theV,1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    
    // The diffuse shading equation
    col = vec4( Ld * Kd * max( dot( s, tnorm ), 0.0 ) , 1.0);
    
    gl_Position = mvp * vec4(theV,1.0);
}

