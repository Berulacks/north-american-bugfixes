#version 330
in vec4 col;
in vec2 tex;

uniform sampler2D texSampler;

void main()
{
    gl_FragColor = texture2D(texSampler, tex) * col;
}
