#version 130
varying vec4 col;
varying vec2 tex;

uniform sampler2D texSampler;

void main()
{
	gl_FragColor = texture2D(texSampler, tex) * col;
}
