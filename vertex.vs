#version 130
//Most valuable player
uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normal_matrix;
uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ld; // Light source intensity

//outgoing color
varying vec4 col;
varying vec2 tex;

//incoming vector
attribute vec3 theV;
//incoming normal
attribute vec3 theN;
//incoming texture coord
attribute vec2 tex_in;

void main()
{

	//col = 10.0 * vec4(theV, 0);
	//col = 10.0 * vec4(theN, 0);
	tex = tex_in;

	// Convert normal and position to eye coords
	vec3 tnorm = normalize( normal_matrix * theN );
	//vec3 tnorm = normalize( toNorm );
	vec4 eyeCoords = mv * vec4(theV,1.0);
	vec3 s = normalize(vec3(LightPosition - eyeCoords));
	
	// The diffuse shading equation
	col = vec4( Ld * Kd * max( dot( s, tnorm ), 0.0 ) , 1.0);
	//col = vec4(theN, 1.0);
	
	gl_Position = mvp * vec4(theV,1.0);
}

