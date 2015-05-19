#version 330
in vec4 col;
in vec2 tex;

uniform sampler2D texSampler;

const float blurSizeH = 1.0 / 300.0;
const float blurSizeV = 1.0 / 200.0;

void main()
{    
    vec4 sum = vec4(0.0);
    for (int x = -4; x <= 4; x++)
        for (int y = -4; y <= 4; y++)
            sum += texture(
                texSampler,
                vec2(tex.x + x * blurSizeH, tex.y + y * blurSizeV)
            ) / 81.0;
    gl_FragColor = sum;
}
