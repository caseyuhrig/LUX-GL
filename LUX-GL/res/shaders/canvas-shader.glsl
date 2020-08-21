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

uniform sampler2D u_ScreenTexture;


void main()
{
    //ivec2 texSize = textureSize(u_ScreenTexture);
    //ivec2 texCoord = ivec2(TexCoords * texSize);
    FragColor = texture(u_ScreenTexture, TexCoords).rgba;
}


