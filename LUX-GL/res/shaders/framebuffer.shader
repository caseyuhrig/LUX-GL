#shader vertex
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}



#shader fragment
#version 450 core
//precision highp float;
//precision highp float;
//precision highp int;
//precision highp sampler2D;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
//uniform sampler2DMS screenTexture;


vec4 MultiSampleTexture(sampler2DMS tex, ivec2 texCoord, int samples)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < samples; i++)
        result += texelFetch(tex, texCoord, i);

    result /= float(samples);
    return result;
}

void main()
{
    //FragColor = MultiSampleTexture(screenTexture, ivec2(TexCoords), 4);
    //FragColor = vec4(1.0,0.0,0.0,1.0);
    FragColor = texture(screenTexture, TexCoords).rgba;
}


