#version 330
in vec4 col;
//We'll just keep this here to prevent too many things from breaking
in vec2 tex;

uniform sampler2D texSampler;

void main()
{
    gl_FragColor = col;
}
