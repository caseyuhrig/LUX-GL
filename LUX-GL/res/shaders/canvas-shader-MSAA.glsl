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
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2DMS u_ScreenTexture;
uniform int u_Samples;


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
    ivec2 texSize = textureSize(u_ScreenTexture);
    ivec2 texCoord = ivec2(TexCoords * texSize);
    gl_FragColor = MultiSampleTexture(u_ScreenTexture, texCoord, u_Samples);
}
